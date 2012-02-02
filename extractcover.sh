COVER_DIR="/d/Songs-Comic/_Covers"

mkdir -p $COVER_DIR
touch $COVER_DIR/index.lst

while [ "$1" ]; do 
    album=`(
    metaflac --show-tag=album "$1" | cut -d= -f2
    metaflac --list --block-type=VORBIS_COMMENT "$1" | gawk -F_album= '/cue_track[0-9]+_album/{print $2}'
    metaflac --show-tag=cuesheet "$1" | gawk -F'"' '/^TITLE/{print $2}/^FILE/{exit}'
    ) |
    /usr/bin/sed -e 's@^ +@@g' -e 's@ +$@@g' -e '/^$/d' |
    /usr/bin/sort -u
    `
    metaflac --export-picture-to="$COVER_DIR/$$.jpg" "$1"
    if [ $? -eq 0 ]; then 
        pushd $COVER_DIR >/dev/null
        md5=`md5sum $$.jpg | cut -d' ' -f1`
        if grep -q $md5 index.lst 2>/dev/null ; then
            echo "Cover for $1 Already Exists" >&2
            rm $$.jpg
        else
            mv $$.jpg $md5.jpg
            echo $md5 $album >>index.lst
        fi
        popd >/dev/null
    else
        echo "Could not extract cover from $1" >&2
    fi
    shift
done
