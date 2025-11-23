import datetime
import glob
import os
import signal
import threading
import time
import queue
import subprocess
from tkinter import *

import TextWindow
import Message
import FileSystemUtility

#______________________________________________________________________________
class DSyncController(Toplevel):
  #____________________________________________________________________________
  def __init__(self, title):
    Toplevel.__init__(self)
    self.title(title)
    self.resizable(0,1)
    self.__make_layout()
    self.protocol('WM_DELETE_WINDOW', self.withdraw)
    self.withdraw()
  #____________________________________________________________________________
  def __make_layout(self):
    font = ('Courier', -12)
    self.status_frame = Frame(self)
    self.status_frame.pack(side=TOP, fill=BOTH, expand=True)
    self.status_text = Text(self.status_frame, font=font, height=10, width=109)
    self.status_text.config(state=DISABLED)
    self.status_text.pack(side=LEFT, fill=BOTH, expand=True)
    self.status_scrollb = Scrollbar(self.status_frame,
                                    command=self.status_text.yview)
    self.status_text.config(yscrollcommand=self.status_scrollb.set)
    self.status_scrollb.pack(side=LEFT, fill=Y)
    self.log_frame = Frame(self)
    self.log_frame.pack(side=TOP, fill=BOTH, expand=True)
    self.log_text = Text(self.log_frame, font=font, height=10, width=109)
    self.log_text.config(state=DISABLED)
    self.log_text.pack(side=LEFT, fill=BOTH, expand=True)
    self.log_scrollb = Scrollbar(self.log_frame, command=self.log_text.yview)
    self.log_text.config(yscrollcommand=self.log_scrollb.set)
    self.log_scrollb.pack(side=LEFT, fill=Y)
  #____________________________________________________________________________
  def deiconify(self):
    self.log_text.see(END)
    Toplevel.deiconify(self)
  #____________________________________________________________________________
  def AddText(self, line):
    pos = self.log_scrollb.get()
    self.log_text.config(state=NORMAL)
    self.log_text.insert(END, line)
    line_end = self.log_text.index(END)
    if line_end > '500.0' :
      self.log_text.delete(1.0, line_end+'-501lines')
    self.log_text.config(state=DISABLED)
    if pos[1] > 0.99:
      self.log_text.see(END)

#______________________________________________________________________________
class RsyncProcess():
  #____________________________________________________________________________
  def __init__(self, src, dest):
    self.src = src
    self.dest = dest
    self.size = os.path.getsize(src)
    self.hsize = FileSystemUtility.natural_size(self.size)
    self.start = time.time()
    self.command = ('exec ionice -c 3 nice -n 19 rsync -a --partial --inplace '
                    + src + ' ' + dest)
    self.__lock = os.path.dirname(dest) + '/dsync.lock'
    self.__devnull = open(os.devnull, 'w')
    if (not os.path.isfile(self.__lock) and
        not FileSystemUtility.compare_file_size(src, dest)):
      open(self.__lock, 'w')
    else:
      self.command = 'exit 1'
    self.__process = subprocess.Popen(self.command, shell=True,
                                      stdout=self.__devnull,
                                      stderr=self.__devnull)
  #____________________________________________________________________________
  def __del__(self):
    if 'rsync' in self.command and os.path.isfile(self.__lock):
      os.remove(self.__lock)
  #____________________________________________________________________________
  def kill(self):
    self.__process.send_signal(signal.SIGKILL)
  #____________________________________________________________________________
  def poll(self):
    return self.__process.poll()
  #____________________________________________________________________________
  def is_synced(self):
    return (self.poll() == 0 and
            FileSystemUtility.compare_file_size(self.src, self.dest))

#______________________________________________________________________________
class DataSynchronizer():
  #____________________________________________________________________________
  def __init__(self, path, primary_path_list, secondary_path_list):
    self.msg_win = DSyncController('Data Synchronizer')
    self.menubar = Menu()
    self.msg_win.config(menu=self.menubar)
    self.menubar.add_command(label='Start', command=self.__start)
    self.menubar.add_command(label='Stop', state=DISABLED)
    self.menubar.add_command(label=' ' * 1, state=DISABLED)
    self.erase_flag = IntVar()
    self.erase_flag.set(0)
    self.menubar.add_checkbutton(label='Erase', onvalue=1, offvalue=0,
                                 variable=self.erase_flag)
    self.menubar.add_command(label=' ' * 3, state=DISABLED)
    self.menubar.add_command(label='IDLE',
                             background='#d9d9d9',
                             activebackground='#d9d9d9',
                             foreground='blue',
                             activeforeground='blue')
    self.menubar.add_command(label=' '*108, state=DISABLED)
    menu_force = Menu(self.menubar, tearoff=0)
    self.menubar.add_cascade(label='Force control', menu=menu_force)
    menu_force.add_command(label='Force erase dsync.lock',
                           command=self.__force_erase_lock)
    self.path = path
    self.primary_path_list = primary_path_list
    self.secondary_path_list = secondary_path_list
    self.message_q = queue.Queue()
    self.proc = []
    self.max_process = len(self.secondary_path_list)
    self.state = 'IDLE'
    if len(self.secondary_path_list) == 0:
      self.menubar.entryconfig('Start', state=DISABLED)
      self.menubar.entryconfig('Stop', state=DISABLED)
  #____________________________________________________________________________
  def __check(self, check_hash=False):
    n_sync = 0
    n_wait = 0
    remain_size = 0
    for src in self.get_data_list():
      r = int(src.replace('.dat', '').replace('.gz', '')[-5:])
      dest = (self.secondary_path_list[r % len(self.secondary_path_list)]
              + '/' + os.path.basename(src))
      if FileSystemUtility.compare_file_size(src, dest):
        n_sync += 1
        if self.erase == 1:
          if check_hash:
            shash = FileSystemUtility.get_hash(src)
            dhash = FileSystemUtility.get_hash(dest)
            if shash == dhash and self.state == 'RUNNING':
              self.__put(f'erased {src}')
          else:
            self.__put(f'erased {src}')
            os.remove(src)
      else:
        n_wait += 1
        remain_size += os.path.getsize(src)
    return n_sync, n_wait, remain_size
  #____________________________________________________________________________
  def __force_erase_lock(self):
    self.__put('Force erase dsync.lock')
    for p in self.secondary_path_list:
      l = p + '/dsync.lock'
      if os.path.isfile(l):
        self.__put(f'erased {l}')
        os.remove(l)
      else:
        self.__put(f'no lock file in {p}')
  #____________________________________________________________________________
  def __put(self, message):
    message = (time.strftime('%Y %m/%d %H:%M:%S')
               + ' ' + message.rstrip() + '\n')
    self.message_q.put(message)
  #____________________________________________________________________________
  def __rsync(self):
    for i, src in enumerate(self.get_data_list()):
      if (self.state != 'RUNNING' or
          os.path.dirname(src) == os.path.realpath(self.path)):
        break
      while len(self.proc) >= self.max_process:
        for j in reversed(xrange(len(self.proc))):
          if self.proc[j].poll() is not None:
            if self.proc[j].is_synced():
              self.__put(f'synced {self.proc[j].src:38} -> '
                         +f'{os.path.dirname(self.proc[j].dest):20} '
                         +f'{self.proc[j].size:>12}\n')
            del self.proc[j]
      r = int(src.replace('.dat', '').replace('.gz', '')[-5:])
      dest = (self.secondary_path_list[r % len(self.secondary_path_list)]
              + '/' + os.path.basename(src))
      self.proc.append(RsyncProcess(src, dest))
      size = FileSystemUtility.natural_size(src)
    while len(self.proc) != 0:
      for j in reversed(xrange(len(self.proc))):
        if self.state != 'RUNNING':
          self.proc[j].kill()
        if self.proc[j].poll() is not None:
          if self.proc[j].is_synced():
            self.__put(f'synced {self.proc[j].src:38} -> '
                       +f'{os.path.dirname(self.proc[j].dest):20} '
                       +f'{self.proc[j].size:>12}\n')
          del self.proc[j]
  #____________________________________________________________________________
  def __run(self):
    self.__put('run_thread start')
    while self.state == 'RUNNING':
      n_sync, n_wait, remain_size = self.__check()
      if n_wait > 0:
        self.__rsync()
      else:
        time.sleep(0.5)
    self.__put('run_thread stop')
  #____________________________________________________________________________
  def __start(self):
    if self.state == 'IDLE':
      self.menubar.entryconfig('Start', state=DISABLED)
      self.menubar.entryconfig('Stop', state=NORMAL, command=self.__stop)
      self.state = 'RUNNING'
      self.menubar.insert_command('IDLE', label='RUNNING',
                          background='#d9d9d9',
                          activebackground='#d9d9d9',
                          foreground='red',
                          activeforeground='red')
      self.menubar.delete('IDLE')
      self.menubar.insert_command('Force control', label=' '*101,
                                  state=DISABLED)
      self.menubar.delete(' '*108)
      self.run_thread = threading.Thread(target=self.__run)
      self.run_thread.setDaemon(True)
      self.run_thread.start()
  #____________________________________________________________________________
  def __stop(self):
    if self.state == 'RUNNING':
      self.menubar.entryconfig('Start', state=NORMAL, command=self.__start)
      self.menubar.entryconfig('Stop', state=DISABLED)
      self.state = 'IDLE'
      self.menubar.insert_command('RUNNING', label='IDLE',
                                  background='#d9d9d9',
                                  activebackground='#d9d9d9',
                                  foreground='blue',
                                  activeforeground='blue')
      self.menubar.delete('RUNNING')
      self.menubar.insert_command('Force control', label=' '*108,
                                  state=DISABLED)
      self.menubar.delete(' '*101)
      self.run_thread.join()
  #____________________________________________________________________________
  def get_message(self):
    linebuf = []
    while not self.message_q.empty():
      linebuf.append(self.message_q.get())
    return linebuf
  #____________________________________________________________________________
  def get_data_list(self):
    data_list = []
    for p in self.primary_path_list:
      if os.path.realpath(self.path) == p:
          continue
      data_list += glob.glob(p + '/*.dat*')
    data_list.sort()
    return data_list
  #____________________________________________________________________________
  def update(self):
    self.erase = self.erase_flag.get()
    message = time.strftime('%Y %m/%d %H:%M:%S')
    message += (f'{"Free(GB)":>28} {"Used(GB)":>8} {"Usage":>8} {"Sync":>7} '
                +f'{"Wait":>6} {"RSize":>10}\n')
    max_usage_p = 0
    max_usage_s = 0
    for p in self.primary_path_list:
      n_sync = 0
      n_wait = 0
      remain_size = 0
      data_list = glob.glob(p + '/*.dat*')
      data_list.sort()
      for i, f in enumerate(data_list):
        r = int(f.replace('.dat', '').replace('.gz', '')[-5:])
        dest = (self.secondary_path_list[r % len(self.secondary_path_list)]
                + '/' + os.path.basename(f))
        if FileSystemUtility.compare_file_size(f, dest):
          n_sync += 1
        else:
          n_wait += 1
          remain_size += os.path.getsize(f)
      free, used, total, usage = FileSystemUtility.get_disk_usage(p)
      max_usage_p = max(max_usage_p, usage)
      message += 'Primary   '
      message += (f'{p:28} {free:8} {used:8}    {usage:5.1%}')
      message += (f'{n_sync:8} {n_wait:6} '
                  +f'{FileSystemUtility.natural_size(remain_size):>10}')
      if os.path.realpath(self.path) == p:
        message += '  @@@ CURRENT @@@'
      message += '\n'
    for p in self.secondary_path_list:
      free, used, total, usage = FileSystemUtility.get_disk_usage(p)
      max_usage_s = max(max_usage_s, usage)
      message += 'Secondary '
      message += f'{p:28} {free:8} {used:8}    {usage:5.1%}'
      message += f'{len(glob.glob(p + "/*.dat*")):8} {"":6} {"":>10}'
      if os.path.isfile(p + '/dsync.lock'):
        message += '  %%% SYNCING %%%'
      message += '\n'
    message += f'{len(self.proc):>3} Processes running\n'
    for p in self.proc:
      elapsed_time = time.time() - p.start
      if not 'rsync' in p.command:
        continue
      src = p.command.split()[-2]
      dest = p.command.split()[-1]
      src_size = FileSystemUtility.natural_size(src)
      dest_size = FileSystemUtility.natural_size(dest)
      speed = FileSystemUtility.file_size(dest) / 1e6 / elapsed_time
      message += (f'Running ... {src:38} -> {os.path.dirname(dest):12} '
                  +f'{dest_size:>9}/{src_size:>9} ({speed:.1f} MB/s)')
      message += '\n'
    if max_usage_p > 0.50 or max_usage_s > 0.75:
      fg_color = 'yellow'
      bg_color = 'black'
      font = ('Courier', -12, 'bold')
      flush = True
    elif max_usage_p > 0.75 or max_usage_s > 0.90:
      fg_color = 'red'
      bg_color = 'black'
      font = ('Courier', -12, 'bold')
      flush = True
    else:
      fg_color = 'black'
      bg_color = 'white'
      font = ('Courier', -12)
      flush = False
    fg = self.msg_win.status_text.cget('fg')
    bg = self.msg_win.status_text.cget('bg')
    if flush and fg == fg_color and bg == bg_color:
      fg_color = bg
      bg_color = fg
    self.msg_win.status_text.config(state=NORMAL, fg=fg_color, bg=bg_color,
                                    font=font)
    self.msg_win.status_text.delete(1.0, END)
    message = message.rstrip()
    for line in message:
      if not line:
        continue
      self.msg_win.status_text.insert(END, line)
    self.msg_win.status_text.config(state=DISABLED)
    self.msg_win.status_text.see(1.0)
  #____________________________________________________________________________
  def AddMessage(self, linebuf):
    for line in linebuf:
      self.msg_win.AddText(line)
  #____________________________________________________________________________
  def SaveMessage(self, logfile, linebuf):
    with open(logfile, 'a') as f:
      for line in linebuf:
        if not line:
          continue
        f.write(line)
  #____________________________________________________________________________
  def AddSaveMessage(self, logfile, linebuf):
    self.AddMessage(linebuf)
    self.SaveMessage(logfile, linebuf)
