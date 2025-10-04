The snap version didnt work well so i rcm you use mozillateam version

#1 Add mozillateam repo
	sudo snap remove firefox 
	Sudo add-apt-repository ppa:mozillateam/ppa -y
	
#2 Priority Mozillateam package
	echo 'Package: firefox
	Pin: release o=Ubuntu*
	Pin-Priority: -1' | sudo tee /etc/apt/preferences.d/firefox-no-snap.pref
	
	echo 'Package: *
	Pin: release o=LP-PPA-mozillateam
	Pin-Priority: 1001' | sudo tee /etc/apt/preferences.d/mozilla-firefox
	
	sudo apt update
	sudo apt install firefox y
	
#3 Check if those command work 
	apt-cache policy firefox
	
	If the result look like this so it worked
	firefox:
  		Installed: (none)
  		Candidate: 142.0.1+build1-0ubuntu0.22.04.1~mt1
  		Version table:
    		 	1:1snap1-0ubuntu2 -1
        	500 http://ports.ubuntu.com/ubuntu-ports jammy/main arm64 Packages
       		1001 /var/lib/dpkg/status
    		 	142.0.1+build1-0ubuntu0.22.04.1~mt1 1001
       		1001 https://ppa.launchpadcontent.net/mozillateam/ppa/ubuntu jammy/main arm64 Packages



