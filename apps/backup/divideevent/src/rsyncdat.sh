#!/bin/bash
# rsyncdat.sh
# 使い方:
#   ./rsyncdat.sh <datファイル名 or 絶対パス> [interval_seconds]
# 例:
#   ./rsyncdat.sh run00044.dat           # デフォルト 30秒ごとに同期
#   ./rsyncdat.sh /root/daq/data/run00044.dat 10

set -o pipefail

# ====== パラメータ ======
SRC_DIR="/root/daq/data"                                  # dat の元置き場（相対パス受け取り時に使用）
DST_DIR="/root/daq/data/divideeventOut/datacopy"          # ローカルのコピー先
LOG_DIR="/root/daq/data/divideeventOut/logs"              # ログ置き場
INTERVAL="${2:-30}"                                       # 何秒ごとに同期するか（第2引数で上書き可）

# リモート
REMOTE_USER="notani"
REMOTE_HOST="login.cc.kek.jp"
REMOTE_DIR="/group/nu/ninja/work/otani/FROST_beamdata/test/datfile/"

# SSH/鍵（2段ジャンプを想定：sshcc1 → login）
SSH_KEY="/root/.ssh/rsync_ed25519"
SSH_OPTS="-o BatchMode=yes -o IdentitiesOnly=yes -o PreferredAuthentications=publickey \
          -o StrictHostKeyChecking=accept-new -o UserKnownHostsFile=/root/.ssh/known_hosts \
          -J sshcc1.kek.jp -i ${SSH_KEY}"

# rsync（リモート送信用）: -a(権限除く) -z(圧縮) --partial --inplace --append-verify
RSYNC_OPTS="-az --partial --inplace --no-perms --chmod=ugo=rwX --append-verify"

# ====== 引数チェック ======
if [ -z "${1:-}" ]; then
  echo "Usage: $0 <dat filename or absolute path> [interval_seconds]" >&2
  exit 1
fi

TARGET_ARG="$1"
# 絶対パスが来たらそれを採用、そうでなければ SRC_DIR から探す
if [[ "$TARGET_ARG" = /* ]]; then
  SRC_PATH="$TARGET_ARG"
  BASENAME="$(basename -- "$TARGET_ARG")"
else
  SRC_PATH="${SRC_DIR}/${TARGET_ARG}"
  BASENAME="$TARGET_ARG"
fi
DST_PATH="${DST_DIR}/${BASENAME}"

# ====== 事前準備 ======
mkdir -p "$DST_DIR" "$LOG_DIR"
log_file="$LOG_DIR/$(date '+%Y-%m-%d').log"

# リモートの送信先ディレクトリを作成（初回）
ssh $SSH_OPTS "${REMOTE_USER}@${REMOTE_HOST}" "mkdir -p '$REMOTE_DIR'" || {
  echo "$(date '+%Y-%m-%d %H:%M:%S'): [FATAL] cannot create remote dir: $REMOTE_DIR" | tee -a "$log_file"
  exit 1
}

echo "$(date '+%Y-%m-%d %H:%M:%S'): Start syncing '$SRC_PATH' -> '$DST_PATH' every ${INTERVAL}s; then to ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}" | tee -a "$log_file"

# ====== ループ ======
while true; do
  # 安全のため毎回ログ日を更新
  log_file="$LOG_DIR/$(date '+%Y-%m-%d').log"

  if [ ! -f "$SRC_PATH" ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): [WARN] source not found: $SRC_PATH" | tee -a "$log_file"
    sleep "$INTERVAL"
    continue
  fi

  # ローカル: 追記分だけコピー（既存部分を検証）→ 速い & 安全
  rsync -t --inplace --append-verify --no-perms --chmod=ugo=rwX \
        "$SRC_PATH" "$DST_PATH"
  if [ $? -eq 0 ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): local copy OK: $SRC_PATH -> $DST_PATH" | tee -a "$log_file"
  else
    echo "$(date '+%Y-%m-%d %H:%M:%S'): [ERROR] local rsync failed; fallback to cp" | tee -a "$log_file"
    if ! cp -f "$SRC_PATH" "$DST_PATH"; then
      echo "$(date '+%Y-%m-%d %H:%M:%S'): [FATAL] cp fallback also failed" | tee -a "$log_file"
      sleep "$INTERVAL"
      continue
    fi
  fi

  # リモート: 同じく追記分だけ送る
  rsync $RSYNC_OPTS \
        -e "ssh $SSH_OPTS" \
        "$DST_PATH" "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/"
  if [ $? -eq 0 ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): remote sync OK: $DST_PATH -> ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/" | tee -a "$log_file"
  else
    echo "$(date '+%Y-%m-%d %H:%M:%S'): [ERROR] remote rsync FAILED" | tee -a "$log_file"
  fi

  sleep "$INTERVAL"
done
