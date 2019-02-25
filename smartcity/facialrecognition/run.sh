hostIp=$1
allApps="facialrecognitionapp"

ddkPath=$DDK_HOME

if [ "$hostIp" = "" ]; then
    echo "Please input the ip."
    exit
fi

if [ "$ddkPath" = "" ]; then
    echo "Please config your DDK_HOME variable."
    exit
fi

dir=${PWD}

cameraPath=$dir/MyDataset\/camera
faceDetectionModel=$dir/MyModel/face_detection_model/device/deploy.om
spherefaceModel=$dir/MyModel/sphereface_model/device/sphereface_deploy.om
vanillacnnModel=$dir/MyModel/vanillacnn_model/device/vanilla_deploy.om

sed -i "s:_camera_data_sets_:$cameraPath:g" ./facialrecognitionapp/graph.config
sed -i "s:_face_detection_model_:$faceDetectionModel:g" ./facialrecognitionapp/graph.config
sed -i "s:_face_feature_mask_model_:$vanillacnnModel:g" ./facialrecognitionapp/graph.config
sed -i "s:_face_recognition_model_:$spherefaceModel:g" ./facialrecognitionapp/graph.config

echo "[INFO]: Begin to build project $dir/$allApps!"
cd $dir/$allApps && sh ./build.sh
libpath=$(echo `cd $ddkPath/../../../lib/; pwd`)
cd $dir

exec $libpath/run.sh -h $1 -n facialrecognitionapp -j $dir/facialrecognitionapp -k $ddkPath -M HIAI -T RUN -e $libpath
