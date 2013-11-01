Mips console
============

.. code:: bash

    #!/usr/bin/expect -f


    set ip 192.168.1.2

    proc powercycle {} {
            send -null 1
            expect "(command-mode)"
            send -- "p"
            #expect "powered on"

            expect {
                    "CFE> " { }
                    -re ".*\r\n"    { send "\003"
                                      exp_continue }
                    }
    }


    set timeout -1
    if {$argc > 1} {
      puts "$argc, $argv"
      puts "usage: mips-console [backend]"
      exit
    }

    if {$argc == 1} {
      set backend $argv
      set spawned [spawn xinu-console -c mips $argv]
    } else {
      set backend null
      set spawned [spawn xinu-console -c mips]
    }
    expect {
            "error: connection not available"
                    { send_user "error: connection not available\r\n"
                      exit }
            -re "connection '(.*)', class '(.*)', host '(.*)'\r\n"
                    { set backend $expect_out(1,string)
                      set class   $expect_out(2,string)
                      set host    $expect_out(3,string)
                    # send_user "connection $backend, class $class, host $host\r\n"
                    }
    }
    sleep 1
    send -null 1
    expect "(command-mode) "
    send -- "d"
    expect "file: "
    send -- "xinu.boot\r"
    expect {
            "download complete\r\n"
                    { }
            "No such file or directory"
                    { send_user "No such file: xinu.boot\r\n"
                      exit }
    }
    sleep 1
    send -- "\r"
    set boot 1
    expect {
            -timeout 1 "CFE> " {set boot 0}
    }
    if {1==$boot} powercycle
    send -- "ifconfig -auto eth0\r"
    expect "CFE> "
    send -- "boot -elf $ip:$backend"
    send -- ".boot\r"
    expect -- "Starting program"
    interact

