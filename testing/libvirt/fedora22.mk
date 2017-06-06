KVM_ISO_URL = http://fedora.bhs.mirrors.ovh.net/linux/releases/22/Server/x86_64/iso/Fedora-Server-DVD-x86_64-22.iso
KVM_KICKSTART_FILE = testing/libvirt/fedora22.ks

KVM_PACKAGE_INSTALL = dnf install -y
KVM_DEBUGINFO_INSTALL = dnf  debuginfo-install -y

KVM_PACKAGES = \
    ElectricFence \
    audit-libs-devel \
    bison \
    conntrack-tools \
    curl-devel \
    fipscheck-devel \
    flex \
    gcc \
    gdb \
    git \
    glibc-devel \
    hping3 \
    hping3 \
    ipsec-tools \
    ldns \
    ldns-devel \
    libcap-ng-devel \
    libevent-devel \
    libseccomp-devel \
    libselinux-devel \
    lsof \
    nc \
    nc6 \
    nsd \
    nss-devel \
    nss-tools \
    nspr-devel \
    ocspd \
    openldap-devel \
    pam-devel \
    pexpect \
    python3-pexpect \
    python3-setproctitle \
    pyOpenSSL \
    racoon2 \
    strace \
    systemd-devel \
    tar \
    unbound \
    unbound-devel \
    unbound-libs \
    valgrind \
    vim-enhanced \
    xl2tpd \
    xmlto \
    https://download.nohats.ca/strongswan/strongswan-5.5.2-1.fc22.x86_64.rpm \
    https://download.nohats.ca/libfaketime/libfaketime-0.9.6-4.fc22.x86_64.rpm

KVM_DEBUGINFO = \
    ElectricFence \
    audit-libs \
    cyrus-sasl \
    glibc \
    keyutils \
    krb5-libs \
    ldns \
    libcap-ng \
    libcom_err \
    libcurl \
    libevent \
    libevent-devel \
    libgcc \
    libidn \
    libseccomp \
    libselinux \
    libssh2 \
    nspr \
    nss \
    nss-softokn \
    nss-softokn-freebl \
    nss-util \
    ocspd \
    openldap \
    openssl-libs \
    pam \
    pcre \
    python-libs \
    sqlite \
    unbound-libs \
    xz-libs \
    zlib