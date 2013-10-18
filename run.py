#! /usr/bin/python
# ex: set ts=4 sw=4 expandtab:

from SimpleHTTPServer import SimpleHTTPRequestHandler
import SocketServer
import subprocess
import sys

class GamedServer(SimpleHTTPRequestHandler):
    def translate_path(self, path):
        if path.endswith('/Login.html'):
            return SimpleHTTPRequestHandler.translate_path(self, 'clients/java/src/gamed/Login.html')
        if path.endswith('/java.jar'):
            return SimpleHTTPRequestHandler.translate_path(self, 'clients/java/dist/java.jar')
        return SimpleHTTPRequestHandler.translate_path(self, path)

    def log_request(*args):
        pass

if __name__ == '__main__':
    port = 8000
    devnull = open('/dev/null', 'w')
    sys.stdout.write("Running Make ........... ")
    try:
        subprocess.check_call(["make"], shell=True, stderr=devnull, stdout=devnull)
        print "OK"
    except:
        print "Fail"
        sys.exit(1)
    sys.stdout.write("Starting Gamed server .. ")
    try:
        subprocess.check_call(["server/server"])
        print "OK"
    except:
        print "Fail"
        sys.exit(1)
    print "Listening on port %i" % port
    SocketServer.TCPServer(("", port), GamedServer).serve_forever()
