release_libhml:
	cd ../evm && make libhml_windows

release_windows:release_libhml
	rm -rf build
	mkdir -p build
	cd build && qmake ../src/evue.pro && make -j4
	cp build/evue-sdl2/evue.exe ./bin/x86_64-window-mingw
	cp ./src/evue-sdl2/SDL2.dll ./bin/x86_64-window-mingw
	cp build/evuesimulator/evuesimulator.exe ./bin/x86_64-window-mingw
	cd ./bin/x86_64-window-mingw && windeployqt.exe evuesimulator.exe

publish_windows:release_windows
	cp test -rf ./bin/x86_64-window-mingw
	cp tools -rf ./bin/x86_64-window-mingw
	cd ./bin/x86_64-window-mingw && 7z a -tzip -r  evuesimulator.zip
	rm -rf ./bin/x86_64-window-mingw/test
	mkdir -p release_zip
	mv ./bin/x86_64-window-mingw/evuesimulator.zip release_zip

release_linux:
	rm -rf build
	mkdir -p build
	cd build && qmake ../src/evue.pro && make -j4
	mkdir -p ./bin/x86_64-linux-gnu/
	cp build/evue-sdl2/evue ./bin/x86_64-linux-gnu
	cp ./src/evue-sdl2/libSDL2-2.0.so.0 ./bin/x86_64-linux-gnu/
	cp build/evuesimulator/evuesimulator ./bin/x86_64-linux-gnu/
	cp -r bin/x86_64-linux-gnu ../evm_publish_final/bin
	cp -r lib/x86_64-linux-gnu ../evm_publish_final/lib

publish_evue:
	mkdir -p release_evue
	cp -rf bin frameworks include lib modules src test tools .gitignore .gitmodules makefile README.md release_evue
	rm -rf release_evue/modules/ecma
	rm -rf release_evue/lvgl
	rm release_evue/src/evue-qt5/evue-qt5.pro
	mv release_evue/src/evue-qt5/evue-qt5-release.pro release_evue/src/evue-qt5/evue-qt5.pro

	rm release_evue/src/evue-sdl2/evue-sdl2.pro
	mv release_evue/src/evue-sdl2/evue-sdl2-release.pro release_evue/src/evue-sdl2/evue-sdl2.pro

	rm -rf release_evue/src/evue.pro.user

	cp -rf release_evue/* ../evue


.PONY: release release_linux