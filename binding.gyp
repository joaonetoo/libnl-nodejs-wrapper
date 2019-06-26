{
  "targets": [
    {
      "target_name": "module",
      "sources": [ "./src/module.c" ],
      "include_dirs": [
        "/usr/include/libnl3"
      ],
      "libraries": [
          "-lnl-cli-3", "-lnl-genl-3", "-lnl-nf-3", "-lnl-route-3", "-lnl-3"
      ]
    }
  ]
}