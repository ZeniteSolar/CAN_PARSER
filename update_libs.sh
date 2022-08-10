LIB_VERSION=`cat .lib_version`
WEB_VERSION=`curl -s https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/.semver`

if [ $LIB_VERSION == $WEB_VERSION ]; then
    echo -e "\033[0;32mCan library is updated\033[0m"
else 
    echo -e "\033[0;33mNew vesion of can library is avalible, \033[0m"
    echo "Do you want to Update (y/n)..."
    read answer
    if [ "$answer" != "${answer#[Yy]}" ] ;then
        echo "Updating..."
        wget https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/can_ids.h -O can_ids.h
        wget https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/can_parser.h -O can_parse.h
        wget https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/.semver -O .lib_version
    else 
        echo -e "\033[0;31mMantain can library out of date is unsafe!\033[0m"
    fi
fi


