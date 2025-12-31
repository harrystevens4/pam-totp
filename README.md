
# About

This is uses an implementation of the TOTP algorithm defined in RFC 6238. It should work with services like google authenticator or microsoft authenticator.
it is an auth service for pam, so can be a drop in replacement for `pam_unix.so` or used in combination.

# Usage

Follow install instructions listed below to install.
Secret keys are stored in the file `/etc/totp_keys` in the format
```
user:secretkey
e.g.
root:A58B3E0FF3==
 ^^^^^^
notice no spaces
```
where the secretkey is a base32 string.

# Install

Compile with `make pam_totp.so`
Install with `./install`
