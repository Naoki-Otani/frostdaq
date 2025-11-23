#!/usr/bin/python3

import os
import signal
import sys
import argparse
import time
import subprocess
import psutil
import re
from dataclasses import dataclass

# ---- Add: Tee for section-scoped logging ----
class _SectionTee:
    def __init__(self, filepath):
        self.filepath = filepath
        self._f = None
        self._orig_out = None
        self._orig_err = None

    def start(self):
        self._f = open(self.filepath, "w", buffering=1)  # 行単位でフラッシュ
        self._orig_out = sys.stdout
        self._orig_err = sys.stderr
        sys.stdout = self
        sys.stderr = self

    def stop(self):
        if self._f:
            self._f.flush()
            self._f.close()
            self._f = None
        if self._orig_out:
            sys.stdout = self._orig_out
            self._orig_out = None
        if self._orig_err:
            sys.stderr = self._orig_err
            self._orig_err = None

    # file-like interface
    def write(self, data):
        # ターミナルにも出しつつ、ファイルにも書く（tee）
        if self._orig_out:
            self._orig_out.write(data)
            self._orig_out.flush()
        if self._f:
            self._f.write(data)
            self._f.flush()

    def flush(self):
        if self._orig_out:
            self._orig_out.flush()
        if self._f:
            self._f.flush()
# ---------------------------------------------


TRGFW_DIR = "/root/apps/rayraw-soft/install/TrgFw"
YAENAMI_DIR = "/root/apps/rayraw-soft/install/YaenamiControl"
RAYRAW_FE_DIR = "/root/apps/hddaq/Frontend/rayraw_node/script"
LAUNCHER_DIR = "/root/daq"
RAYRAW_CONTROL_DIR = "/root/apps/rayraw-control"
ONLINE_ANALYZER_DIR = "/root/apps/online-v9"
HUL_DIR = "/root/apps/hul-common-lib/install"
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = f"{LAUNCHER_DIR}/data"
HVLOG_PATH = f"{SCRIPT_DIR}/HV_LOG.txt"
HV_SLEEP_TIME = 0.5
MAX_HV = 100
ASICSTATE_LOG_PATH = f"{SCRIPT_DIR}/ASICSTATE_LOG.txt"

boards = []

@dataclass
class Board:
	board_num: int
	nickname: str
	port: int
	ip: str
	nodeid: int
	is_connected: bool
	def __init__(self, board_num):
		self.num = board_num
		self.nickname = get_nickname(board_num)
		self.port = get_port(board_num)
		self.ip = get_ip_address(board_num)
		self.nodeid =  get_nodeid(board_num)
		self.is_connected = False

def signal_handler(sig, frame):
	global boards
	if boards:
		print("\nInterrupted by user. Exiting...", flush=True)
		kill()
	sys.exit(0)

def get_ip_address(board_num):
	return f"192.168.20.{100 + board_num}"

def get_port(board_num):
	return 9000 + board_num

def get_nickname(board_num):
	return f"rayraw-{board_num}"

def get_nodeid(board_num):
	return 0x100 + board_num

def kill():
	print("Stopping the frontend script...", end="")
	os.chdir(RAYRAW_FE_DIR)
	os.system("./fe_kill.sh > /dev/null 2> /dev/null")
	print(" Done!")

	print("Stopping the run control monitor...", end="")
	os.chdir(LAUNCHER_DIR)
	os.system("./kill.sh > /dev/null 2> /dev/null")
	print(" Done!")

	#print("Turning off MPPC HV...")
	turn_off_hv()
	#print(" Done!")

	os.chdir(SCRIPT_DIR)
	sys.exit(0)

def kill_proc(pattern, dryrun=False):
	for proc in psutil.process_iter(['pid', 'name']):
		if pattern in proc.info['name']:
			cmd = f"kill -9 {proc.info['pid']}"
			if dryrun:
				print(f"[Dryrun] {cmd}")
			else:
				print(f"Killing process: {cmd}")
				os.system(cmd)

def turn_off_hv():
	try:
		with open(HVLOG_PATH, "r") as f:
			lines = f.readlines()
	except FileNotFoundError:
		print(f"No HV log file at {HVLOG_PATH} found. Assuming no HV is applied.")
		lines = []
	for i in range(len(lines)):
		board_num = i + 1
		line = lines[i].strip()
		applied_hv = int(line)
		if applied_hv > 0:
			for hv in [int(x) for x in range(applied_hv, 256, 50)]+[0]:
				apply_hv(board_num, hv)
			print(f"Turned off MPPC HV for RAYRAW #{board_num}! (initial HV: {applied_hv})")

def log_voltage(board_num, hv):
	if board_num < 1:
		raise ValueError("board_num must be greater than 0")
	try:
		with open(HVLOG_PATH, "r") as f:
			lines = f.readlines()
	except FileNotFoundError:
		lines = []
		
	while len(lines) < board_num:
		lines.append(f"0\n")
	
	lines[board_num-1] = f"{hv}\n"

	with open(HVLOG_PATH, "w") as f:
		f.writelines(lines)

def apply_hv(board_num, hv):
	cmd = f"{HUL_DIR}/bin/set_max1932 {get_ip_address(board_num)} {hv}"
	#print(cmd)
	os.system(cmd)
	log_voltage(board_num, hv)
	time.sleep(HV_SLEEP_TIME)

def get_current_hv(board_num):
	try:
		with open(HVLOG_PATH, "r") as f:
			lines = f.readlines()
		if 0 < board_num <= len(lines):
			return int(lines[board_num - 1].strip())
		else:
			return 0
	except FileNotFoundError:
		return 0
	
def parse_asic_status(hex_status):
	binary_status = bin(int(hex_status, 16))[2:].zfill(4)
	asic_map = ["U16", "U17", "U18", "U19"]
	is_ready = [board for bit, board in zip(binary_status[::-1], asic_map) if bit == '1']
	return binary_status, is_ready

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

parser = argparse.ArgumentParser(description="Run DAQ script for RAYRAW boards.")
parser.add_argument("boards", nargs="?", default="1,2,3,4,5,6,7,8,9,10,11,12", help="Comma-separated list of board numbers (e.g., 1,2,3)")
parser.add_argument("--debug", action="store_true", help="Enable debug mode")
parser.add_argument("--min_window", type=int, default=50, help="Minimum window size for frontend script")
parser.add_argument("--max_window", type=int, default=450, help="Maximum window size for frontend script")
parser.add_argument("--ringbuf", type=int, default=100000, help="Ring buffer size for launcher")
parser.add_argument("--ringbuf_len", type=int, default=100, help="Length of the ring buffer for launcher")
hv_choices = list(range(MAX_HV, 256))
hv_choices.append(0)
parser.add_argument("--hv", type=int, default=125, choices=hv_choices, metavar="[MAX_HV-255]", help=f"MPPC HV input (smaller the value, higher the voltage, MAX_HV={MAX_HV})")
parser.add_argument("--REGISTER_VTH_COMP", type=int, default=520, choices=range(0, 1024), metavar="[0-1023]", help="Comparator threshold voltage")
parser.add_argument("--reset_fpga", action="store_true", help="Reset FPGA before starting the frontend script")
parser.add_argument("--kill", action="store_true", help="Kill the frontend script and exit")
parser.add_argument("--data_path",  default="", help="Path to store data files")
args = parser.parse_args()

if args.debug:
	print("Running in debug mode")

sys.argv = [sys.argv[0], args.boards]

board_numbers = []
for part in sys.argv[1].split(','):
	part = part.strip()
	if part.isdigit():
		num = int(part)
		if 1 <= num <= 12:
			board_numbers.append(num)
		else:
			print(f"Invalid board number (must be 1-12): {part}")
			sys.exit(1)
	else:
		print(f"Invalid board number: {part}")
		sys.exit(1)

############### 1. Check board connection ###############

print("\n" + "#" * 8 + " 1. Check connection " + "#" * 8 + "\n")

ping_results = {}

for board_num in board_numbers:
	ip = get_ip_address(board_num)
	while True:
		print(f"Checking connection to RAYRAW #{board_num} ({ip}) ... ", end="", flush=True)
		response = os.system(f"ping -c 1 -W 5 {ip} > /dev/null 2>&1")
		if response == 0:
			print("OK!")
			ping_results[board_num] = True
			break
		print(f"\nRAYRAW #{board_num}: Press force reset switch or check board connection with the given IP {ip}!", flush=True)
		choice = input("Do you want to skip this board? [y/n]: ").strip().lower()
		if choice == 'y':
			print(f"Skipping RAYRAW #{board_num}...")
			ping_results[board_num] = False
			break
		#else:
		#    print(f"Retrying board {ip}...")

board_numbers = [board for board, success in ping_results.items() if success]
print("\nBoards to be used for DAQ:", board_numbers)

boards = [Board(board_num) for board_num in board_numbers]

if args.kill:
	kill()

if not boards:
	print("No board is connected. Aborting!")
	sys.exit(1) 

############### 2. Write FPGA RegisterValues ###############

print("\n" + "#" * 8 + " 2. Write FPGA register values " + "#" * 8 + "\n")

choice = input("Are the boards just turned on and need to be initialized? [y/n]: ").strip().lower()
if choice == 'y':
	rv_seq = ["initialize", "normal"]
else:
	rv_seq = ["normal"]

os.chdir(RAYRAW_CONTROL_DIR)
print("\nCompiling register values...", flush=True)

for key in list(args.__dict__.keys()):
	if key.startswith("REGISTER_"):
		value = getattr(args, key)
		reg_name = key.replace("REGISTER_", "")
		#if not (0 <= value <= 1023):
		#    print(f"Invalid value for {key}: {value} (must be between 0 and 1023)")
		#    sys.exit(1)
		print(f"{reg_name} = {value}", flush=True)
		with open(f"{RAYRAW_CONTROL_DIR}/setup/setup_{reg_name}.txt", "w") as f:
			for ch in range(0, 32):
				f.write(f"{ch} : {value}\n")

os.system("./bin/rayraw_control")
os.chdir(SCRIPT_DIR)
print("")

for board in boards:
	if args.reset_fpga:
		print(f"Resetting RAYRAW #{board.num} FPGA... ", end="", flush=True)
		os.system(f"{HUL_DIR}/bin/reconfig_fpga {board.ip} > /dev/null 2> /dev/null &")
	for mode in rv_seq:
		cmd = f"{YAENAMI_DIR}/bin/set_asic_register {board.ip} {YAENAMI_DIR}/RegisterValue {mode}"
		if args.debug:
			ret = os.system(cmd)
		else:
			ret = os.system(f"{cmd} > /dev/null 2>&1")
		if ret != 0:
			print(f"Failed to write register values for RAYRAW #{board.num} ({mode})", flush=True)
		elif mode == "normal":
			print(f"Successfully wrote register values for RAYRAW #{board.num}", flush=True)




############### 3. Start RAYRAW frontend script ###############

print("\n" + "#" * 8 + " 3. Start RAYRAW frontend script " + "#" * 8 + "\n")

# ここから③のログだけをダンプ
_frontend_tee = _SectionTee(ASICSTATE_LOG_PATH)
_frontend_tee.start()

os.chdir(RAYRAW_FE_DIR)
#subprocess.Popen("./message.sh", shell=True, cwd=RAYRAW_FE_DIR)
os.system("./message.sh > /dev/null 2> /dev/null &")
#os.system("./message.sh &")
time.sleep(1)

asic_stat_pattern = re.compile(r"#D: AdcRo IsReady status: ([0-9a-fA-F])")

proc_fe = {}

def read_proc_fe_output(proc, board_num):
	for line in proc.stdout:
		print(f"[RAYRAW #{board_num}] {line!r}", flush=True)


print("                       Nickname \tNode\tPort\tIP Address\tASIC Ready Status", flush=True)
for board in boards:
	target_cmd = f"./frontend_rayraw.sh {board.nickname} {board.nodeid} {board.port} {board.ip} {args.min_window} {args.max_window}"
	cmd = f"{target_cmd} >/dev/null 2>/dev/null &"
	#cmd = f"{target_cmd} &"
	result = subprocess.run(["ps", "-ef"], stdout=subprocess.PIPE, text=True)

	if target_cmd in result.stdout:
		print(f"    Running RAYRAW #{board.num:<2}: {board.nickname:<9}\t{board.nodeid}\t{board.port}\t{board.ip}", flush=True)
	else:
		print(f"Starting up RAYRAW #{board.num:<2}: {board.nickname:<9}\t{board.nodeid}\t{board.port}\t{board.ip}", end="", flush=True)
		if args.debug:
			print(f"DEBUG: {cmd}")
		
		try:
			#print(target_cmd)
			trgfw_cmd = f"./bin/adcro {board.ip}"
			proc_fe[board.num] = subprocess.run(trgfw_cmd, shell=True, cwd=TRGFW_DIR, capture_output=True, text=True)
			match = asic_stat_pattern.search(proc_fe[board.num].stdout)
			if match:
				hex_status = match.group(1)
				binary_status, is_ready = parse_asic_status(hex_status)
				print(f"\t{is_ready}")
		except Exception as e:
			print(f"Error while reading output for RAYRAW #{board.num}: {e}")

		os.system(cmd)

os.chdir(SCRIPT_DIR)

# ③のログダンプはここまで
_frontend_tee.stop()

############### 4. Apply MPPC HV ###############

print("\n" + "#" * 8 + " 4. Apply MPPC HV " + "#" * 8 + "\n")

for board in boards:
	initial_hv = 255
	current_hv = get_current_hv(board.num)
	if current_hv > 0:
		print(f"MPPC HV already applied for RAYRAW #{board.num}. Current HV: {current_hv}")
		initial_hv = current_hv
		continue
	for hv in [int(x) for x in range(initial_hv, args.hv-1, -50)]:
		apply_hv(board.num, hv)
	apply_hv(board.num, args.hv)
	print(f"Applied MPPC HV {args.hv} to RAYRAW #{board.num}")



############### 5. Open launcher ###############

print("\n" + "#" * 8 + " 5. Open launcher " + "#" * 8 + "\n")

if args.data_path != "":
	if not os.path.exists(args.data_path):
		print(f"Creating data directory at {args.data_path}...")
		os.makedirs(args.data_path)

	if os.path.lexists(DATA_DIR):
		org_link = DATA_DIR
		if not os.path.samefile(args.data_path, DATA_DIR):
			if os.path.islink(DATA_DIR):
				org_link = os.readlink(DATA_DIR)
				print(f"Overwriting existing symlink with source at {org_link}...")
				os.remove(DATA_DIR)
			else:
				raise FileExistsError(f"{DATA_DIR} is not a symlink!")

	os.symlink(args.data_path, DATA_DIR)
	print(f"Created data symlink with source at {args.data_path}!")

else:
	if not os.path.exists(DATA_DIR):
		os.makedirs(DATA_DIR)
	print(f"Using data path {DATA_DIR} with source at {os.readlink(DATA_DIR)}!")

with open(f"{LAUNCHER_DIR}/datanode.txt", "w") as nodetxt:
	for board in boards:
		nodetxt.write(f"localhost\t{board.port}\t{args.ringbuf}\t{args.ringbuf_len}\n")


proc_launcher = subprocess.Popen(["./launcher.py"], cwd=LAUNCHER_DIR)

# os.chdir(LAUNCHER_DIR)
# os.system(f"./launcher.py > /dev/null 2> /dev/null &")
# os.chdir(SCRIPT_DIR)



############### 6. Open online analyzer ###############

# print("\n" + "#" * 8 + " 6. Open online analyzer " + "#" * 8 + "\n")

# os.chdir(ONLINE_ANALYZER_DIR)
# os.system(f"./bin/raw_hist_rayraw ../param/conf/ninja_rayraw.conf localhost:8901")
# os.chdir(SCRIPT_DIR)

# proc_online = subprocess.Popen(["./bin/raw_hist_rayraw", "../param/conf/ninja_rayraw.conf", "localhost:8901"], cwd=ONLINE_ANALYZER_DIR)

# processes = [proc_launcher, proc_online]
processes = [proc_launcher]
try:
	while True:
		for proc in processes:
			if proc.poll() is not None:
				print(f"Process \"{' '.join(proc.args)}\" was interrupted. Aborting...")
				for other in processes:
					if other != proc and other.poll() is None:
						print(f"Killing process \"{' '.join(other.args)}\"...")
						other.terminate()

				for proc in processes:
					proc.wait()

				kill()
				sys.exit(0)

		time.sleep(0.5)

except KeyboardInterrupt:
	print("KeyboardInterrupt received. Exiting...")
	for proc in processes:
		proc.terminate()
	sys.exit(1)
