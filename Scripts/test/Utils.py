import requests
import sys
import os
import winreg

import time
import urllib

def DownloadFile(Url,FilePath):

    Path=os.path.abspath(FilePath)
    os.makedirs(os.path.dirname(Path),exist_ok=True)
    
    with open(Path, 'wb') as f:
        headers= {"User-Agen": "MOzilla/Chrome/Safari"}
        r = requests.get(Url,headers=headers,stream=True)
        total = r.headers.get("content-length")
        if total == None:
            f.write(r.content)
        else:
            download=0
            procent=0
            total= int(total)
            startTime= time.time()
            for data in r.iter_content(chunk_size=max(int(total/1000),1024*1024)):
                download+= len(data)
                procent=download/total
                f.write(data)
                sys.stdout.write('\rdownloading... {:.2f}%'.format(procent*100))
                sys.stdout.flush()
            sys.stdout.write('\n')