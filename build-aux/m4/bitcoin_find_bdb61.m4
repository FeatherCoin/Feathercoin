AC_DEFUN([BITCOIN_FIND_BDB61],[
  AC_MSG_CHECKING([for Berkeley DB C++ headers])
  BDB_CPPFLAGS=
  BDB_LIBS=
  bdbpath=X
  bdb61path=X
  bdbdirlist=
  for _vn in 6.1 61 6 ''; do
    for _pfx in b lib ''; do
      bdbdirlist="$bdbdirlist ${_pfx}db${_vn}"
    done
  done
  for searchpath in $bdbdirlist ''; do
    test -n "${searchpath}" && searchpath="${searchpath}/"
    AC_TRY_COMPILE([
      #include <${searchpath}db_cxx.h>
    ],[
      #if !((DB_VERSION_MAJOR == 6 && DB_VERSION_MINOR >= 1) || DB_VERSION_MAJOR > 6)
        #error "failed to find bdb 6.1+"
      #endif
    ],[
      if test "x$bdbpath" = "xX"; then
        bdbpath="${searchpath}"
      fi
    ],[
      continue
    ])
    AC_TRY_COMPILE([
      #include <${searchpath}db_cxx.h>
    ],[
      #if !(DB_VERSION_MAJOR == 6 && DB_VERSION_MINOR == 1)
        #error "failed to find bdb 6.1"
      #endif
    ],[
      bdb61path="${searchpath}"
      break
    ])
  done
  if test "x$bdbpath" = "xX"; then
    AC_MSG_RESULT([no])
    AC_MSG_ERROR(libdb_cxx headers missing)
  elif test "x$bdb61path" = "xX"; then
    BITCOIN_SUBDIR_TO_INCLUDE(BDB_CPPFLAGS,[${bdbpath}],db_cxx)
    AC_ARG_WITH([incompatible-bdb],[AS_HELP_STRING([--with-incompatible-bdb], [allow using a bdb version other than 6.1])],[
      AC_MSG_WARN([Found Berkeley DB other than 6.1; wallets opened by this build will not be portable!])
    ],[
      AC_MSG_ERROR([Found Berkeley DB other than 6.1, required for portable wallets (--with-incompatible-bdb to ignore)])
    ])
  else
    BITCOIN_SUBDIR_TO_INCLUDE(BDB_CPPFLAGS,[${bdb61path}],db_cxx)
    bdbpath="${bdb61path}"
  fi
  AC_SUBST(BDB_CPPFLAGS)
  
  # TODO: Ideally this could find the library version and make sure it matches the headers being used
  for searchlib in db_cxx-6.1 db_cxx; do
    AC_CHECK_LIB([$searchlib],[main],[
      BDB_LIBS="-l${searchlib}"
      break
    ])
  done
  if test "x$BDB_LIBS" = "x"; then
      AC_MSG_ERROR(libdb_cxx missing)
  fi
  AC_SUBST(BDB_LIBS)
])
