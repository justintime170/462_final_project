import time
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import string
import socket
import subprocess
from multiprocessing import Process
import sys

prevLine = "0"
stringBuffer = []
sckt = None
listen = True


def getLastLine(direct):
    logFile = open(direct, 'r')
    lineList = logFile.readlines()
    lastLine = lineList[len(lineList)-1]
    return lastLine

def waitForCommands():
    try:
        print "Waiting for commands"
        while True:
            num =""
            num = sckt.recv(1)
            if(len(num) != 0):
                print num
                if(num == '1'):
                    subprocess.call(['curl','http://127.0.0.1/mailbox/sl'])#curl command for left turn
                else:
                    if(num == '2'):
                        subprocess.call(['curl','http://127.0.0.1/mailbox/f'])#curl command for forward
                    else:
                        if(num == '3'):
                            subprocess.call(['curl','http://127.0.0.1/mailbox/sr'])#curl command for right turn
                        else:
                            if(num == '4'):
                                subprocess.call(['curl','http://127.0.0.1/mailbox/b'])#curl command for back
                            else:
                                if(num == '5'):
                                    subprocess.call(['curl','http://127.0.0.1/mailbox/fl'])#curl command for 90 degree right turn
                                else:
                                    if(num == '6'):
                                        subprocess.call(['curl','http://127.0.0.1/mailbox/fr'])#curl command for 90 degree left turn
                                    else:
                                        if(num == '7'):
                                            subprocess.call(['curl','http://127.0.0.1/mailbox/st'])#curl command for stop
                                        else:
                                            if(num == '8'):
                                                subprocess.call(['curl','http://127.0.0.1/mailbox/st'])
                                                sckt.close()
                                                break
    except socket.error as serr:
        if serr.errno == 131:
            print "Client disconnected, waiting for new client\n"
            subprocess.call(['curl','http://127.0.0.1/mailbox/dis'])
            #print "ending thread"



class MyHandler(FileSystemEventHandler):
    def on_modified(self, event):
        if event.src_path == "./distlog.txt":
            global prevLine
            global stringBuffer
            pathLst = string.split(event.src_path, "/")
            if len(pathLst) > 0:
                path = pathLst[len(pathLst)-1]
                lastLine = getLastLine(path)
                if lastLine != prevLine:
                    prevLine = lastLine
                    stringBuffer.append(lastLine)


if __name__ == "__main__":
    event_handler = MyHandler()
    observer = Observer()
    observer.schedule(event_handler, path='.', recursive=False)
    observer.start()

    #global stringBuffer
    global sckt
    global listen
    s = socket.socket()
    host = socket.gethostname()
    port = 9000
    s.bind(('', port))

    try:
        s.listen(5)
        while True:
            try:
                print "Waiting for connection on port " + str(port) + "..."
                sckt, addr = s.accept()
                print 'Got connection from', addr
                #sckt.send('Thank you for connecting')
                subprocess.call(['curl','http://127.0.0.1/mailbox/con'])
                p1 = Process(target=waitForCommands, args=())
                p1.start()
                while True:
                    buflen = len(stringBuffer)
                    if not p1.is_alive():
                        #print "P1 is dead\n"
                        break
                    if buflen > 0:
                        print "Items waiting in buffer: " + str(buflen) + "\n"
                        item = stringBuffer[0]
                        print item
                        sckt.send(item + "\n")
                        stringBuffer.remove(item)
                        print "Items now waiting in buffer: " + str(len(stringBuffer)) + "\n"

                print "Client Disconnected Gracefully"
                subprocess.call(['curl','http://127.0.0.1/mailbox/dis'])
            except socket.error as serr:
                if serr.errno == 131:
                    print "Client disconnected, waiting for new client :(\n"
                    subprocess.call(['curl','http://127.0.0.1/mailbox/dis'])
    except KeyboardInterrupt:
        observer.stop()
        subprocess.call(['curl','http://127.0.0.1/mailbox/st'])
        subprocess.call(['curl','http://127.0.0.1/mailbox/dis'])
    observer.join()