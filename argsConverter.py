import win32clipboard

win32clipboard.OpenClipboard()
str = win32clipboard.GetClipboardData().split(" ")[1:]
args = ""

for arg in str:
    args += "\"" + arg + "\","

args = args[:-1]

win32clipboard.EmptyClipboard()
win32clipboard.SetClipboardText(args)
win32clipboard.CloseClipboard()