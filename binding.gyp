{
  "targets": [
    {
      "target_name": "route_tools",
      "sources": [
        "src/interfaces.h",
      ],
      'conditions': [
        ['OS=="win"',
          {
            'sources': [
              "src/route_tools_win.c"
            ],
            'include_dirs+':
            [
              # Not needed now
            ]
          }
        ],
        ['OS=="linux"',
          {
            'sources': [
              "./src/route_tools_linux.c"
            ],
            "include_dirs+": [
                 "/usr/include/libnl3"
            ],

            'link_settings': {
                    "libraries": [
                      "-lnl-cli-3", "-lnl-genl-3", "-lnl-nf-3", "-lnl-route-3", "-lnl-3"
                    ]
            }
          }
        ]
      ]
    }
  ]
}
