TEMPLATE = lib

CONFIG += thread

HOME=$$system(echo $HOME) 

# install the license file
lic.path = $${HOME}/OTA/docs/LICENSE
lic.files = COPYING
INSTALLS += lic

# install the translation files
i18n.path = $${HOME}/OTA/docs/i18n
i18n.files = ../i18n/*.qm
INSTALLS += i18n

