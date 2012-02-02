#!/usr/bin/python
from optparse import OptionParser
import subprocess
import string
import os
import os.path
import sys
import shutil

def convert(source_fn,dest_fn,maxres):
    src=file(source_fn,'rb')
    dst=file(dest_fn,'wb')
    ret=subprocess.call('mogrify -format jpg -resize ">^%d" -'%maxres,
                        stdin=src,stdout=dst,shell=True)
    src.close()
    dst.close()
    return ret

def main():
    parser=OptionParser(usage='usage: %prog [options] file')
    parser.add_option('-c','--cover',dest='cover',help='specify cover art')
    parser.add_option('-d','--dir',dest='dir',help='specify booklet directory')
    parser.add_option('','--max-res',dest='maxres',help='max resolution of booklet, default 2000',default=2000)
    parser.add_option('','--cover-res',dest='coverres',help='max resolution of cover, default 1000',default=1000)

    opt,arg=parser.parse_args()

    if len(arg)==0:
        print "Error: No flac file specified"
        parser.print_help()
        return -1

    opt.filename=arg[0]

#{'coverres': 1000, 'cover': None, 'maxres': 2000, 'dir': None, 'filename': 'file.flac'}

    command="metaflac "

    if opt.cover!=None:
        if convert(opt.cover,'cover-converted.jpg',opt.coverres):
            print "Error: Cannot convert %s"%opt.cover
            return -1
        command+='--import-picture-from="3||Front Cover||cover-converted.jpg" '

    if opt.dir!=None:
        try:
            shutil.rmtree('converted')
        except OSError,e:
            pass
        count=0
        try:
            os.mkdir('converted')
            for i in os.listdir(opt.dir):
                basename=i.rsplit('.',1)[0]
                if convert("%s/%s"%(opt.dir,i),'converted/%s.jpg'%basename,opt.maxres):
                    print "Warning: cannot convert %s/%s"%(opt.dir,i)
                else:
                    command+='--import-picture-from="0||%s||converted/%s.jpg" '%(count,basename)
            count+=1

        except OSError:
            print "Error: Cannot access directory",opt.dir
            return -1

    command+='"'+opt.filename+'"'
    if subprocess.call(command,shell=True):
        print "Error: failed inserting metadata into flac"
        return -1

    shutil.rmtree('converted')
    os.remove('cover-converted.jpg')
    return 0

if __name__=='__main__':
    exit(main())
