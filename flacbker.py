#!/usr/bin/python
from optparse import OptionParser
import subprocess
import string
import os

def shell(command):
    return subprocess.Popen(command,shell=True).wait()

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
        exit(-1)

    opt.filename=arg[0]

#{'coverres': 1000, 'cover': None, 'maxres': 2000, 'dir': None, 'filename': 'file.flac'}

    command="metaflac "

    if opt.cover!=None:
        if shell('convert -resize ">^%d" %s cover-converted.jpg'
            %(opt.coverres,opt.cover)):
            print "Error: Cannot convert %s"%opt.cover
            exit(-1)
        command+='--import-picture-from="3||Front Cover||cover-converted.jpg" '

    if opt.dir!=None:
        try:
            os.rmdir('converted')
        except OSError:
            pass
        try:
            os.mkdir('converted')
            for i in os.listdir(opt.dir):
                basename=string.split(i,'.')[0]
                if shell('convert -resize ">^%d" %s/%s converted/%s.jpg'
                    %(opt.maxres,opt.dir,i,basename)):
                    print "Warning: cannot convert %s/%s"%(opt.dir,i)
                else:
                    command+='--import-picture-from="0||%s||converted/%s.jpg" '%(basename,basename)

        except OSError:
            print "Error: Cannot access directory",opt.dir
            exit(-1)

    command+=opt.filename
    if shell(command):
        print "Error: failed inserting metadata into flac"
        exit(-1)

if __name__=='__main__':
    main()
