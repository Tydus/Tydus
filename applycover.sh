COVER_DIR="/d/Songs-Comic/_Covers"

while [ "$1" ]; do 
    album=`(
    metaflac --show-tag=album "$1" | cut -d= -f2
    metaflac --list --block-type=VORBIS_COMMENT "$1" | gawk -F_album= '/cue_track[0-9]+_album/{print $2}'
    metaflac --show-tag=cuesheet "$1" | gawk -F'"' '/^TITLE/{print $2}/^FILE/{exit}'
    ) |
    /usr/bin/sed -e 's@^ +@@g' -e 's@ +$@@g' -e '/^$/d' |
    head -n1
    `

    if metaflac --list --block-type=PICTURE "$1" | grep -q PICTURE; then
        echo "The file $1 already have a picture"
    else
        fn="$COVER_DIR/`grep "$album" $COVER_DIR/index.lst | cut -c-32`.jpg"

        if [ -f "$fn" ]; then
            metaflac --import-picture-from="$fn" "$1"
        else
            echo "Warning: don't have cover cached for $1"
        fi
    fi
    shift
done
