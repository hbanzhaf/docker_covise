This image provides the dependencies that are necessary to compile COVISE along with OddLOT.

To build this image, execute:

```bash
docker build . --network=host -t docker-covise
```

To run this image, execute:
```bash
export UID && export GID=$(id -g) && docker-compose -f docker-compose.yml run --rm docker-covise
```

Afterwards, the build process can be started by executing the following commands inside the container:
```bash
git clone https://github.com/hlrs-vis/covise.git --recursive ~/Downloads/covise
cd ~/Downloads/covise/
source .covise.sh
mkdir -p ${ARCHSUFFIX}/build.covise
cd ${ARCHSUFFIX}/build.covise
cmake ../.. -DCOVISE_BUILD_ONLY_ODDLOT=ON -DCOVISE_WARNING_IS_ERROR=OFF
cd ${COVISEDIR}
make -j$(nproc)
```

OddLOT can finally be started by running:
```bash
~/Downloads/covise/bin/oddlot
```
