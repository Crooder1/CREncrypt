# Overview
CREncrypt is a simple and lightweight file encryption command line tool with the goal of allowing users to encrypt files with a password. CREncrypt works for both Linux and Windows, and is recommended to add to ./usr/bin (Linux) or PATH environment variable (Windows) for convenience.

## How To Use

From -help flag:
`Usage: crencrypt <-(encrypt/e)/(decrypt/d)> [-(n/nopreserve)/(p/preserve)] <key>  <infile> [out]`
`-encrypt -e: encrypt mode`
`-decrypt -d: decrypt mode`
`-preserve -p (OPTIONAL): preserve the infile`
`-nopreserve -n (OPTIONAL): safe delete the infile`
`key: the characters used to encrypt`
`infile: the input file`
`outfile (OPTIONAL): the output file`

Breakdown:

- The -encrypt and -decrypt flags are used to switch between encryption and decryption modes.
- The -preserve and -nopreserve are used to indicate whether the input file should be kept. If nopreserve is selected, it will be wiped from disc. Default is preserve but optional.
- The key parameter is the password to the file
- Infile and outfile are self explanatory. Outfile is optional however, and will default to [infile].enc or [infile].dec depending on mode, if not provided.


