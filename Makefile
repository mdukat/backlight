VERSION=1.0
REVISION=1

OUT=backlight
SOURCE=main.c

#ARCH=$(shell uname -m)
ARCH=amd64
DEBNAME=$(OUT)_$(VERSION)-$(REVISION)_$(ARCH)

$(OUT): $(SOURCE)
	gcc -o $(OUT) $(SOURCE)

install: $(OUT)
	cp $(OUT) /usr/local/bin/
	chown root:root /usr/local/bin/$(OUT)
	chmod u+s /usr/local/bin/$(OUT)

deb: $(OUT)
	mkdir -p $(DEBNAME)/usr/local/bin
	cp $(OUT) $(DEBNAME)/usr/local/bin/
	mkdir $(DEBNAME)/DEBIAN
	# Generate control file
	echo "Package: $(OUT)" > $(DEBNAME)/DEBIAN/control
	echo "Version: $(VERSION)" >> $(DEBNAME)/DEBIAN/control
	echo "Architecture: $(ARCH)" >> $(DEBNAME)/DEBIAN/control
	echo "Maintainer: Mateusz Dukat <mateusz.dukat1@gmail.com>" >> $(DEBNAME)/DEBIAN/control
	echo "Description: Ultra-simple Intel Backlight control tool" >> $(DEBNAME)/DEBIAN/control
	# Generate postinst file
	echo "#!/bin/sh" > $(DEBNAME)/DEBIAN/postinst
	echo "chmod u+s /usr/local/bin/$(OUT)" >> $(DEBNAME)/DEBIAN/postinst
	chmod 0775 $(DEBNAME)/DEBIAN/postinst
	# Build package
	dpkg-deb --build --root-owner-group $(DEBNAME)

clean:
	rm $(OUT)
	rm -rf $(DEBNAME) $(DEBNAME).deb
