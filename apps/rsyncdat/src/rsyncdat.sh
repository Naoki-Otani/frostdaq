#!/bin/bash
# rsyncdat.sh  (nocache/帯域/Dirty監視入り)
# 使い方:
#   ./rsyncdat.sh <datファイル名 or 絶対パス> [interval_seconds]
# 例:
#   ./rsyncdat.sh run00044.dat
#   BW_LIMIT=30M ./rsyncdat.sh /root/daq/data/run00044.dat 10
#
# 改善点:
# - rsync を nocache でラップ（ローカル読取 & リモート書込の両側）。ページキャッシュ滞留を最小化
# - --append / --inplace / --no-compress を維持（追記専用前提）
# - Dirty/Writeback をログに吐いて観測（“溜まりっぱなし”を検知）
# - 必要に応じて --bwlimit と vm.dirty_* を bytes 指定で絞れる可変パラメータ

set -o pipefail

# ====== ユーザ調整可能パラメータ ======
SRC_DIR="/root/daq/data"                    # 相対パス指定時の検索元
LOG_DIR="/root/apps/rsyncdat/logs"
INTERVAL="${2:-30}"                         # 同期間隔（秒）

# リモート
REMOTE_USER="notani"
REMOTE_HOST="login.cc.kek.jp"
REMOTE_DIR="/group/nu/ninja/work/otani/FROST_beamdata/e71c/datfile"

# SSH/鍵（2段ジャンプ: sshcc1 → login）
SSH_KEY="/root/.ssh/rsync_ed25519"
SSH_OPTS="-o BatchMode=yes -o IdentitiesOnly=yes -o PreferredAuthentications=publickey \
          -o StrictHostKeyChecking=accept-new -o UserKnownHostsFile=/root/.ssh/known_hosts \
          -o ConnectTimeout=10 \
          -o ServerAliveInterval=60 -o ServerAliveCountMax=5 \
          -J sshcc1.kek.jp -i ${SSH_KEY}"

# 帯域制限（任意）。環境変数 BW_LIMIT でも上書き可（例: 30M / 200M）
BW_LIMIT="${BW_LIMIT:-}"

# nocache を使うか（既定: 使う）。インストールされていなければ自動フォールバック
USE_NOCACHE="${USE_NOCACHE:-1}"

# Dirty のハード/背景上限を bytes で軽く絞るか（任意）
#   0=何もしない / 1=設定する
# 値は環境変数で上書き可:
#   DIRTY_BACKGROUND_BYTES, DIRTY_BYTES
TUNE_DIRTY_LIMITS="${TUNE_DIRTY_LIMITS:-0}"
DIRTY_BACKGROUND_BYTES="${DIRTY_BACKGROUND_BYTES:-536870912}"  # 512MB
DIRTY_BYTES="${DIRTY_BYTES:-1610612736}"                       # 1.5GB

# ====== rsync オプション ======
RSYNC_APPEND_OPTS=(-a --partial --inplace --append --no-perms --chmod=ugo=rwX --no-compress --timeout=120)
RSYNC_FULL_OPTS=(-a --partial --inplace --no-perms --chmod=ugo=rwX --no-compress --timeout=120)

# ====== 引数チェック・パス決定 ======
if [ -z "${1:-}" ]; then
  echo "Usage: $0 <dat filename or absolute path> [interval_seconds]" >&2
  exit 1
fi
TARGET_ARG="$1"
if [[ "$TARGET_ARG" = /* ]]; then
  SRC_PATH="$TARGET_ARG"
  BASENAME="$(basename -- "$TARGET_ARG")"
else
  SRC_PATH="${SRC_DIR}/${TARGET_ARG}"
  BASENAME="$TARGET_ARG"
fi

# ====== 事前準備 ======
mkdir -p "$LOG_DIR"
log_file="$LOG_DIR/$(date '+%Y-%m-%d').log"

# nocache 検出
if [[ "$USE_NOCACHE" = "1" ]] && command -v nocache >/dev/null 2>&1; then
  NOCACHE_LOCAL=(nocache -n --)
  NOCACHE_REMOTE="rsync"
else
  NOCACHE_LOCAL=()
  NOCACHE_REMOTE="rsync"
fi

# 帯域オプション
RSYNC_BWLIMIT=()
if [[ -n "$BW_LIMIT" ]]; then
  RSYNC_BWLIMIT=(--bwlimit="$BW_LIMIT")
fi

# リモート送信先ディレクトリ作成（初回）
if ! ssh $SSH_OPTS "${REMOTE_USER}@${REMOTE_HOST}" "mkdir -p '$REMOTE_DIR'"; then
  echo "$(date '+%Y-%m-%d %H:%M:%S'): [FATAL] cannot create remote dir: $REMOTE_DIR" | tee -a "$log_file"
  exit 1
fi

# （任意）Dirty 上限を bytes で設定
if [[ "$TUNE_DIRTY_LIMITS" = "1" ]]; then
  if [[ $EUID -ne 0 ]]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): [WARN] TUNE_DIRTY_LIMITS=1 ですが root でないため vm.dirty_* を変更できません" | tee -a "$log_file"
  else
    sysctl -w vm.dirty_background_bytes="$DIRTY_BACKGROUND_BYTES" >/dev/null
    sysctl -w vm.dirty_bytes="$DIRTY_BYTES" >/dev/null
    echo "$(date '+%Y-%m-%d %H:%M:%S'): set vm.dirty_background_bytes=$(printf "%'d" "$DIRTY_BACKGROUND_BYTES"), vm.dirty_bytes=$(printf "%'d" "$DIRTY_BYTES")" | tee -a "$log_file"
  fi
fi

echo "$(date '+%Y-%m-%d %H:%M:%S'): Start direct rsync '$SRC_PATH' → ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR} every ${INTERVAL}s" | tee -a "$log_file"
if ((${#NOCACHE_LOCAL[@]})); then
  echo "$(date '+%Y-%m-%d %H:%M:%S'): nocache enabled (local)" | tee -a "$log_file"
fi
if [[ -n "$BW_LIMIT" ]]; then
  echo "$(date '+%Y-%m-%d %H:%M:%S'): bwlimit = $BW_LIMIT" | tee -a "$log_file"
fi

trap 'echo "$(date "+%Y-%m-%d %H:%M:%S"): caught signal, exit." | tee -a "$log_file"; exit 0' INT TERM

# 直前の状態（サイズ/inode）で追記か判断
prev_size=""
prev_ino=""

# ====== 同期ループ ======
while true; do
  log_file="$LOG_DIR/$(date '+%Y-%m-%d').log"

  if [ ! -f "$SRC_PATH" ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): [WARN] source not found: $SRC_PATH" | tee -a "$log_file"
    sleep "$INTERVAL"
    continue
  fi

  # stat 取得
  cur_size="$(stat -c '%s' -- "$SRC_PATH" 2>/dev/null || echo "")"
  cur_ino="$(stat -c '%i' -- "$SRC_PATH" 2>/dev/null || echo "")"

  # append/full 判定
  mode="append"
  if [ -z "$prev_size" ] || [ -z "$prev_ino" ]; then
    mode="append"
  elif [ "$cur_ino" != "$prev_ino" ]; then
    mode="full"     # ローテーション等
  elif [ "$cur_size" -lt "$prev_size" ]; then
    mode="full"     # truncate 等
  fi

  # 実行（DAQ 優先: 低優先度 I/O/CPU）
  if [ "$mode" = "append" ]; then
    ionice -c3 -t nice -n 10 \
      "${NOCACHE_LOCAL[@]}" \
      rsync "${RSYNC_APPEND_OPTS[@]}" \
	    "${RSYNC_BWLIMIT[@]}" \
            -e "ssh $SSH_OPTS" \
            --rsync-path="$NOCACHE_REMOTE" \
            -- "$SRC_PATH" "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/"
    rc=$?
    action="APPEND"
  else
    ionice -c3 -t nice -n 10 \
      "${NOCACHE_LOCAL[@]}" \
      rsync "${RSYNC_FULL_OPTS[@]}" "${RSYNC_BWLIMIT[@]}" \
            -e "ssh $SSH_OPTS" \
            --rsync-path="$NOCACHE_REMOTE" \
            -- "$SRC_PATH" "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/"
    rc=$?
    action="FULL"
  fi

  # ログ出力 + Dirty/Writeback の観測
  if [ $rc -eq 0 ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): remote sync OK ($action): $SRC_PATH -> ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/$BASENAME" | tee -a "$log_file"
    prev_size="$cur_size"
    prev_ino="$cur_ino"
  else
    echo "$(date '+%Y-%m-%d %H:%M:%S'): [ERROR] rsync FAILED ($action). rc=$rc" | tee -a "$log_file"
  fi

  # Dirty/Writeback の瞬間値を記録（詰まり検出の目安）
  if [ -r /proc/meminfo ]; then
    dirty=$(grep -E '^Dirty:' /proc/meminfo | awk '{print $2$3}')
    writeback=$(grep -E '^Writeback:' /proc/meminfo | awk '{print $2$3}')
    echo "$(date '+%Y-%m-%d %H:%M:%S'): meminfo Dirty=$dirty, Writeback=$writeback" | tee -a "$log_file"
  fi

  sleep "$INTERVAL"
done
