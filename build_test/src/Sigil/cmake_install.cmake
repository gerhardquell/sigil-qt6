# Install script for directory: /data1/u2/sigil074/src/Sigil

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/sigil" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/sigil")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/sigil"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/sigil")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/data1/u2/sigil074/build_test/bin/sigil")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/sigil" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/sigil")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/sigil"
         OLD_RPATH "/usr/local/Qt-6.9.2/gcc_64/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/sigil")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/applications/sigil.desktop")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/share/applications" TYPE FILE FILES "/data1/u2/sigil074/src/Sigil/Resource_Files/freedesktop/sigil.desktop")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/pixmaps/sigil.png")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/share/pixmaps" TYPE FILE RENAME "sigil.png" FILES "/data1/u2/sigil074/src/Sigil/Resource_Files/icon/app_icon_48.png")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/sigil/translations/sigil_ca.qm;/usr/local/share/sigil/translations/sigil_cs.qm;/usr/local/share/sigil/translations/sigil_da_DK.qm;/usr/local/share/sigil/translations/sigil_de.qm;/usr/local/share/sigil/translations/sigil_el_GR.qm;/usr/local/share/sigil/translations/sigil_en.qm;/usr/local/share/sigil/translations/sigil_es.qm;/usr/local/share/sigil/translations/sigil_fi.qm;/usr/local/share/sigil/translations/sigil_fr.qm;/usr/local/share/sigil/translations/sigil_gl.qm;/usr/local/share/sigil/translations/sigil_hu_HU.qm;/usr/local/share/sigil/translations/sigil_id_ID.qm;/usr/local/share/sigil/translations/sigil_is.qm;/usr/local/share/sigil/translations/sigil_it.qm;/usr/local/share/sigil/translations/sigil_ja.qm;/usr/local/share/sigil/translations/sigil_kk.qm;/usr/local/share/sigil/translations/sigil_km.qm;/usr/local/share/sigil/translations/sigil_ko.qm;/usr/local/share/sigil/translations/sigil_lv_LV.qm;/usr/local/share/sigil/translations/sigil_nl.qm;/usr/local/share/sigil/translations/sigil_pl.qm;/usr/local/share/sigil/translations/sigil_pt.qm;/usr/local/share/sigil/translations/sigil_pt_BR.qm;/usr/local/share/sigil/translations/sigil_ro_RO.qm;/usr/local/share/sigil/translations/sigil_ru.qm;/usr/local/share/sigil/translations/sigil_sk.qm;/usr/local/share/sigil/translations/sigil_tr_TR.qm;/usr/local/share/sigil/translations/sigil_ug.qm;/usr/local/share/sigil/translations/sigil_uk.qm;/usr/local/share/sigil/translations/sigil_uk_UA.qm;/usr/local/share/sigil/translations/sigil_vi.qm;/usr/local/share/sigil/translations/sigil_vi_VN.qm;/usr/local/share/sigil/translations/sigil_zh_CN.qm;/usr/local/share/sigil/translations/sigil_zh_TW.qm")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/share/sigil/translations" TYPE FILE FILES
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_ca.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_cs.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_da_DK.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_de.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_el_GR.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_en.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_es.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_fi.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_fr.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_gl.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_hu_HU.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_id_ID.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_is.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_it.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_ja.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_kk.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_km.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_ko.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_lv_LV.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_nl.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_pl.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_pt.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_pt_BR.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_ro_RO.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_ru.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_sk.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_tr_TR.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_ug.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_uk.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_uk_UA.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_vi.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_vi_VN.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_zh_CN.qm"
    "/data1/u2/sigil074/build_test/src/Sigil/sigil_zh_TW.qm"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/sigil/hunspell_dictionaries/About.txt;/usr/local/share/sigil/hunspell_dictionaries/de_DE.aff;/usr/local/share/sigil/hunspell_dictionaries/de_DE.dic;/usr/local/share/sigil/hunspell_dictionaries/en_GB.aff;/usr/local/share/sigil/hunspell_dictionaries/en_GB.dic;/usr/local/share/sigil/hunspell_dictionaries/en_US.aff;/usr/local/share/sigil/hunspell_dictionaries/en_US.dic;/usr/local/share/sigil/hunspell_dictionaries/es.aff;/usr/local/share/sigil/hunspell_dictionaries/es.dic;/usr/local/share/sigil/hunspell_dictionaries/fr.aff;/usr/local/share/sigil/hunspell_dictionaries/fr.dic;/usr/local/share/sigil/hunspell_dictionaries/hyph_de_DE.dic;/usr/local/share/sigil/hunspell_dictionaries/hyph_en_GB.dic;/usr/local/share/sigil/hunspell_dictionaries/hyph_en_US.dic;/usr/local/share/sigil/hunspell_dictionaries/hyph_es.dic;/usr/local/share/sigil/hunspell_dictionaries/hyph_fr.dic")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/share/sigil/hunspell_dictionaries" TYPE FILE FILES
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/About.txt"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/de_DE.aff"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/de_DE.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/en_GB.aff"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/en_GB.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/en_US.aff"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/en_US.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/es.aff"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/es.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/fr.aff"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/fr.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/hyph_de_DE.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/hyph_en_GB.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/hyph_en_US.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/hyph_es.dic"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/dictionaries/hyph_fr.dic"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/sigil/examples/clip_entries.ini;/usr/local/share/sigil/examples/search_entries.ini")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/share/sigil/examples" TYPE FILE FILES
    "/data1/u2/sigil074/src/Sigil/Resource_Files/examples/clip_entries.ini"
    "/data1/u2/sigil074/src/Sigil/Resource_Files/examples/search_entries.ini"
    )
endif()

