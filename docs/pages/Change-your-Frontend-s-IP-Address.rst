Change your Frontend's IP Address
=================================

To change the value of a network device's static IP address find the
network device's configuration file and change the line
`` IPADDR=xxx.xxx.xxx.xxx `` to `` IPADDR=yyy.yyy.yyy.yyy `` where the
*x* 's represent the old value of the static IP address and *y* 's
represent the value you want your IP address to be.

Most likely if you want to change your frontend machine's IP address for
connecting to your backend router it is because your frontend's IP
addres is already configured to be 192.168.1.1 (which conflicts with the
backend router's default IP address of 192.168.1.1) or some other value
outside of the acceptable range of 192.168.1.[2-255]. On our machine we
use the `` eth0 `` device as our network connection to the outside
world, but we reserve the `` eth1 `` device for our network connection
with our backend router. So we want to configure `` eth1 `` to have an
appropriate static IP address but do not want to change any settings for
the `` eth0 `` device. On our machine, the configuration file
`` ifcfg-eth1 `` we're looking for is located in the path
`` /etc/sysconfig/network-scripts/ifcfg-eth1 ``. We opened this file and
obtained the following output:

::

    # ADMtek NC100 Network Everywhere Fast Ethernet 10/100
    DEVICE=eth1
    BOOTPROTO=static
    BROADCAST=192.168.1.255
    HWADDR=00:04:5a:8f:69:89
    IPADDR=192.168.1.1
    NETMASK=255.255.255.0
    NETWORK=192.168.1.0
    ONBOOT=yes
    SEARCH="mscs.mu.edu"
    NM_CONTROLLED=

Notice that the value of the variable `` IPADDR `` is set to
192.168.1.1. This is an unacceptable value, so we want to change it. On
your machine if this value is outside of the acceptable range of
192.168.1.[2-255] then you will want to change it to a value that is in
the given range. The value 192.168.1.2 works well as a default and is
the value we used for the HOWTO:Deploy_Xinu tutorial. So simply change
that line to an address in the acceptable range and save it. It should
now look something like this:

::

    # ADMtek NC100 Network Everywhere Fast Ethernet 10/100
    DEVICE=eth1
    BOOTPROTO=static
    BROADCAST=192.168.1.255
    HWADDR=00:04:5a:8f:69:89
    IPADDR=192.168.1.2
    NETMASK=255.255.255.0
    NETWORK=192.168.1.0
    ONBOOT=yes
    SEARCH="mscs.mu.edu"
    NM_CONTROLLED=

Now your IP address should be configured to work with the backend
router. Make sure to remember this address because you will need it when
the backend is getting the `` xinu.boot `` image from the frontend
machine across this network connection.

Category:HOWTO
