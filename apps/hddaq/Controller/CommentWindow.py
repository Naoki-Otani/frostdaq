import time

import TextWindow

comment_win = TextWindow.TextWindow('Comment Window')

#______________________________________________________________________________
def AddSaveComment(logfile, runno, line):
  comment  = time.strftime('%Y %m/%d %H:%M:%S')
  comment += f' [RUN {runno:05d}] '
  comment += line + '\n'
  comment_win.AddText(comment)
  with open(logfile, 'a') as f:
    f.write(comment)

#______________________________________________________________________________
def ShowComment(line):
  comment_win.AddText(line)

#______________________________________________________________________________
def GetLastComment():
  text = comment_win.GetLastText()
  text = text[40:-1]
  return text
