#!/bin/bash
# rsyncdatnew.sh
# 使い方:
#   ./rsyncdatnew.sh <datファイル名 or 絶対パス> [interval_seconds]
# 例:
#   ./rsyncdatnew.sh run00044.dat           # デフォルト 30秒ごとに同期
#   ./rsyncdatnew.sh /root/daq/data/run00044.dat 10
#
# ポイント:
# - --append-verify を廃止し、追記専用ファイルは --append で末尾のみ同期
# - ローカル中間コピーを廃止し、SRC→REMOTE を直接 rsync
# - ファイルがローテーション/縮小されたらフル同期に自動フォールバック

set -o pipefail

# ====== パラメータ ======
SRC_DIR="/root/daq/data"                    # 相対パス指定時の検索元
LOG_DIR="/root/daq/data/divideeventOut/logs"
INTERVAL="${2:-30}"                         # 同期間隔（秒）

# リモート
REMOTE_USER="notani"
REMOTE_HOST="login.cc.kek.jp"
REMOTE_DIR="/group/nu/ninja/work/otani/FROST_beamdata/test/datfile"

# SSH/鍵（2段ジャンプ: sshcc1 → login）
SSH_KEY="/root/.ssh/rsync_ed25519"
SSH_OPTS="-o BatchMode=yes -o IdentitiesOnly=yes -o PreferredAuthentications=publickey \
          -o StrictHostKeyChecking=accept-new -o UserKnownHostsFile=/root/.ssh/known_hosts \
          -J sshcc1.kek.jp -i ${SSH_KEY}"

# rsync オプション
# 追記パス用（高速・低メモリ／途中書換なし前提）
RSYNC_APPEND_OPTS="-a --partial --inplace --append --no-perms --chmod=ugo=rwX --no-compress"
# フル同期フォールバック用（サイズ縮小/ローテーション時）
RSYNC_FULL_OPTS="-a --partial --inplace --no-perms --chmod=ugo=rwX --no-compress"

# ====== 引数チェック ======
if [ -z "${1:-}" ]; then
  echo "Usage: $0 <dat filename or absolute path> [interval_seconds]" >&2
  exit 1
fi

TARGET_ARG="$1"
# 絶対パスならそのまま、相対なら SRC_DIR に付与
if [[ "$TARGET_ARG" = /* ]]; then
  SRC_PATH="$TARGET_ARG"
  BASENAME="$(basename -- "$TARGET_ARG")"
else
  SRC_PATH="${SRC_DIR}/${TARGET_ARG}"
  BASENAME="$TARGET_ARG"
fi

# ====== 準備 ======
mkdir -p "$LOG_DIR"
log_file="$LOG_DIR/$(date '+%Y-%m-%d').log"

# リモート送信先ディレクトリ作成（初回）
if ! ssh $SSH_OPTS "${REMOTE_USER}@${REMOTE_HOST}" "mkdir -p '$REMOTE_DIR'"; then
  echo "$(date '+%Y-%m-%d %H:%M:%S'): [FATAL] cannot create remote dir: $REMOTE_DIR" | tee -a "$log_file"
  exit 1
fi

echo "$(date '+%Y-%m-%d %H:%M:%S'): Start direct rsync '$SRC_PATH' → ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR} every ${INTERVAL}s" | tee -a "$log_file"

# 終了処理
trap 'echo "$(date "+%Y-%m-%d %H:%M:%S"): caught signal, exit."; exit 0' INT TERM

# 直前の状態（サイズ/inode）を記録しておき、追記かどうか判定
prev_size=""
prev_ino=""

while true; do
  # 日毎ログローテ
  log_file="$LOG_DIR/$(date '+%Y-%m-%d').log"

  if [ ! -f "$SRC_PATH" ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): [WARN] source not found: $SRC_PATH" | tee -a "$log_file"
    sleep "$INTERVAL"
    continue
  fi

  # 現在の stat
  # GNU stat 前提 (%s: size, %i: inode)
  cur_size="$(stat -c '%s' -- "$SRC_PATH" 2>/dev/null || echo "")"
  cur_ino="$(stat -c '%i' -- "$SRC_PATH" 2>/dev/null || echo "")"

  # 判定: 初回 or inode 変化 or サイズ縮小 → フル同期、それ以外 → 追記同期
  mode="append"
  if [ -z "$prev_size" ] || [ -z "$prev_ino" ]; then
    mode="append"   # 初回は append でも問題なし（存在しなければ全体送られる）
  elif [ "$cur_ino" != "$prev_ino" ]; then
    mode="full"     # ローテーション等で別ファイル化
  elif [ "$cur_size" -lt "$prev_size" ]; then
    mode="full"     # 何らかの理由で縮小（truncate 等）
  fi

  # 実行（低優先度で DAQ を妨げない）
  if [ "$mode" = "append" ]; then
    ionice -c3 -t nice -n 10 \
      rsync $RSYNC_APPEND_OPTS -e "ssh $SSH_OPTS" \
        -- "$SRC_PATH" "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/"
    rc=$?
    action="APPEND"
  else
    ionice -c3 -t nice -n 10 \
      rsync $RSYNC_FULL_OPTS -e "ssh $SSH_OPTS" \
        -- "$SRC_PATH" "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/"
    rc=$?
    action="FULL"
  fi

  if [ $rc -eq 0 ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): remote sync OK ($action): $SRC_PATH -> ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/$BASENAME" | tee -a "$log_file"
    # 正常時のみ状態を更新
    prev_size="$cur_size"
    prev_ino="$cur_ino"
  else
    echo "$(date '+%Y-%m-%d %H:%M:%S'): [ERROR] rsync FAILED ($action). rc=$rc" | tee -a "$log_file"
    # 失敗時は状態を更新しない（次回も判定やり直し）
  fi

  sleep "$INTERVAL"
done
