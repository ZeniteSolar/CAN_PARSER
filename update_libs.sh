download_libs () {
    echo "Updating..."
    wget  -q --show-progress https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/can_ids.h -O can_ids.h
    wget  -q --show-progress https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/can_parser_types.h -O can_parser_types.h
    wget  -q --show-progress https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/.semver -O .lib_version
}
FORCE=0
while [[ $# -gt 0 ]]; do
  case $1 in
    -f|--force)
        FORCE=1
        shift
        ;;
    -h|--help)
        echo "Usage: ./update_libs.sh [OPTION]"
        echo "Can library version manager"
        echo " -f       force update"
        exit 0
        ;;
    -*|--*)
        echo "Unknown option $1"
        exit 1
        ;;
    *)
        POSITIONAL_ARGS+=("$1") # save positional arg
        shift # past argument
        ;;
  esac
done

if [[ $FORCE -eq 1 ]]; then
    download_libs
    exit 0
fi

LIB_VERSION=`cat .lib_version`
WEB_VERSION=`curl -s https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/.semver`

if [ $LIB_VERSION == $WEB_VERSION ]; then
    echo -e "\033[0;32mCan library is updated\033[0m"
else 
    echo -e "\033[0;33mNew vesion of can library is avalible, \033[0m"
    
    echo "Do you want to Update (y/n)..."
    read answer
    if [ "$answer" != "${answer#[Yy]}" ] ;then
        download_libs
    else 
        echo -e "\033[0;31mMantain can library out of date is unsafe!\033[0m"
    fi
fi


