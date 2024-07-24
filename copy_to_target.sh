
#!/bin/bash

if [ -z "$1" ]
  then
    echo "No Arguments (Target Device IPv4 Address"
    echo "Usage : ./copy_to_target.sh <IP x.x.x.x>"
    exit 1
fi

#copy execution file to the remote
echo "copying all binary files to target board"
sshpass -p 'temppwd' scp -p -r ./bin/armhf/ debian@"$1":/home/debian/bin
echo "copied"