Release Process
====================

* Update translations, see [translation_process.md](https://github.com/wagerr/wagerr/blob/master/doc/translation_process.md#synchronising-translations).

* Update manpages, see [gen-manpages.sh](https://github.com/wagerr/wagerr/blob/master/contrib/devtools/README.md#gen-manpagessh).

Before every minor and major release:

* Update [bips.md](bips.md) to account for changes since the last release.
* Update version in `configure.ac` (don't forget to set `CLIENT_VERSION_IS_RELEASE` to `true`)
* Write release notes (see below)
* Update `src/chainparams.cpp` nMinimumChainWork with information from the getblockchaininfo rpc.
* Update `src/chainparams.cpp` defaultAssumeValid with information from the getblockhash rpc.
  - The selected value must not be orphaned so it may be useful to set the value two blocks back from the tip.
  - Testnet should be set some tens of thousands back from the tip due to reorgs there.
  - This update should be reviewed with a reindex-chainstate with assumevalid=0 to catch any defect
     that causes rejection of blocks in the past history.

Before every major release:

* Update hardcoded [seeds](/contrib/seeds/README.md). TODO: Give example PR for Wagerr
* Update [`BLOCK_CHAIN_SIZE`](/src/qt/intro.cpp) to the current size plus some overhead.
* Update `src/chainparams.cpp` chainTxData with statistics about the transaction count and rate. Use the output of the RPC `getchaintxstats`, see
  [this pull request](https://github.com/wagerr/wagerr/pull/12270) for an example. Reviewers can verify the results by running `getchaintxstats <window_block_count> <window_last_block_hash>` with the `window_block_count` and `window_last_block_hash` from your output.
* Update version of `contrib/gitian-descriptors/*.yml`: usually one'd want to do this on master after branching off the release - but be sure to at least do it before a new major release

## Building

### First time / New builders

Install Guix using one of the installation methods detailed in
[contrib/guix/INSTALL.md](/contrib/guix/INSTALL.md).

Check out the source code in the following directory hierarchy.

    cd /path/to/your/toplevel/build
    git clone https://github.com/wagerr/guix.sigs.git
    git clone https://github.com/wagerr/wagerr-detached-sigs.git
    git clone https://github.com/wagerr/wagerr.git

### Write the release notes

Open a draft of the release notes for collaborative editing at https://github.com/wagerr/wagerr-devwiki/wiki.

For the period during which the notes are being edited on the wiki, the version on the branch should be wiped and replaced with a link to the wiki which should be used for all announcements until `-final`.

Generate the change log. As this is a huge amount of work to do manually, there is the `list-pulls` script to do a pre-sorting step based on github PR metadata. See the [documentation in the README.md](https://github.com/wagerr/wagerr-maintainer-tools/blob/master/README.md#list-pulls).

Generate list of authors:

    git log --format='- %aN' v(current version, e.g. 0.20.0)..v(new version, e.g. 0.20.1) | sort -fiu

### Setup and perform Guix builds

Checkout the Wagerr version you'd like to build:

```sh
pushd ./wagerr
SIGNER='(your builder key, ie bluematt, sipa, etc)'
VERSION='(new version without v-prefix, e.g. 0.20.0)'
git fetch origin "v${VERSION}"
git checkout "v${VERSION}"
popd
```

Ensure your guix.sigs are up-to-date if you wish to `guix-verify` your builds
against other `guix-attest` signatures.

```sh
git -C ./guix.sigs pull
```

### Create the macOS SDK tarball (first time, or when SDK version changes)

Create the macOS SDK tarball, see the [macdeploy
instructions](/contrib/macdeploy/README.md#deterministic-macos-dmg-notes) for
details.

### Build and attest to build outputs

Follow the relevant Guix README.md sections:
- [Building](/contrib/guix/README.md#building)
- [Attesting to build outputs](/contrib/guix/README.md#attesting-to-build-outputs)

### Verify other builders' signatures to your own (optional)

- [Add other builders keys to your gpg keyring, and/or refresh keys](/contrib/builder-keys/README.md)
- [Verifying build output attestations](/contrib/guix/README.md#verifying-build-output-attestations)

### Commit your non codesigned signature to guix.sigs

```sh
pushd ./guix.sigs
git add "${VERSION}/${SIGNER}"/noncodesigned.SHA256SUMS{,.asc}
git commit -m "Add attestations by ${SIGNER} for ${VERSION} non-codesigned"
git push  # Assuming you can push to the guix.sigs tree
popd
```

## Codesigning

### macOS codesigner only: Create detached macOS signatures (assuming [signapple](https://github.com/achow101/signapple/) is installed and up to date with master branch)

    tar xf wagerr-osx-unsigned.tar.gz
    ./detached-sig-create.sh /path/to/codesign.p12
    Enter the keychain password and authorize the signature
    signature-osx.tar.gz will be created

### Windows codesigner only: Create detached Windows signatures

    tar xf wagerr-win-unsigned.tar.gz
    ./detached-sig-create.sh -key /path/to/codesign.key
    Enter the passphrase for the key when prompted
    signature-win.tar.gz will be created

### Windows and macOS codesigners only: test code signatures
It is advised to test that the code signature attaches properly prior to tagging by performing the `guix-codesign` step.
However if this is done, once the release has been tagged in the wagerr-detached-sigs repo, the `guix-codesign` step must be performed again in order for the guix attestation to be valid when compared against the attestations of non-codesigner builds.

### Windows and macOS codesigners only: Commit the detached codesign payloads

```sh
pushd ./wagerr-detached-sigs
# checkout the appropriate branch for this release series
rm -rf ./*
tar xf signature-osx.tar.gz
tar xf signature-win.tar.gz
git add -A
git commit -m "point to ${VERSION}"
git tag -s "v${VERSION}" HEAD
git push the current branch and new tag
popd
```

### Non-codesigners: wait for Windows and macOS detached signatures

- Once the Windows and macOS builds each have 3 matching signatures, they will be signed with their respective release keys.
- Detached signatures will then be committed to the [wagerr-detached-sigs](https://github.com/wagerr/wagerr-detached-sigs) repository, which can be combined with the unsigned apps to create signed binaries.

### Create the codesigned build outputs

- [Codesigning build outputs](/contrib/guix/README.md#codesigning-build-outputs)

### Verify other builders' signatures to your own (optional)

- [Add other builders keys to your gpg keyring, and/or refresh keys](/contrib/builder-keys/README.md)
- [Verifying build output attestations](/contrib/guix/README.md#verifying-build-output-attestations)

### Commit your codesigned signature to guix.sigs (for the signed macOS/Windows binaries)

```sh
pushd ./guix.sigs
git add "${VERSION}/${SIGNER}"/all.SHA256SUMS{,.asc}
git commit -m "Add attestations by ${SIGNER} for ${VERSION} codesigned"
git push  # Assuming you can push to the guix.sigs tree
popd
```

## After 3 or more people have guix-built and their results match

Combine the `all.SHA256SUMS.asc` file from all signers into `SHA256SUMS.asc`:

```bash
cat "$VERSION"/*/all.SHA256SUMS.asc > SHA256SUMS.asc
```


- Upload to the wagerr.org server (`/var/www/bin/wagerr-${VERSION}/`):
    1. The contents of each `./wagerr/guix-build-${VERSION}/output/${HOST}/` directory, except for
       `*-debug*` files.

       Guix will output all of the results into host subdirectories, but the SHA256SUMS
       file does not include these subdirectories. In order for downloads via torrent
       to verify without directory structure modification, all of the uploaded files
       need to be in the same directory as the SHA256SUMS file.

       The `*-debug*` files generated by the guix build contain debug symbols
       for troubleshooting by developers. It is assumed that anyone that is
       interested in debugging can run guix to generate the files for
       themselves. To avoid end-user confusion about which file to pick, as well
       as save storage space *do not upload these to the wagerr.org server,
       nor put them in the torrent*.

       ```sh
       find guix-build-${VERSION}/output/ -maxdepth 2 -type f -not -name "SHA256SUMS.part" -and -not -name "*debug*" -exec scp {} user@wagerr.org:/var/www/bin/wagerr-${VERSION} \;
       ```

    2. The `SHA256SUMS` file

    3. The `SHA256SUMS.asc` combined signature file you just created

- Create a torrent of the `/var/www/bin/wagerr-${VERSION}` directory such
  that at the top level there is only one file: the `wagerr-${VERSION}`
  directory containing everything else. Name the torrent
  `wagerr-${VERSION}.torrent` (note that there is no `-core-` in this name).

  Optionally help seed this torrent. To get the `magnet:` URI use:

  ```sh
  transmission-show -m <torrent file>
  ```

  Insert the magnet URI into the announcement sent to mailing lists. This permits
  people without access to `wagerr.org` to download the binary distribution.
  Also put it into the `optional_magnetlink:` slot in the YAML file for
  wagerr.org.

- Update other repositories and websites for new version

  - wagerr.org blog post

  - wagerr.org maintained versions update:
    [table](https://github.com/wagerr/wagerr.org/commits/master/_includes/posts/maintenance-table.md)

  - Delete post-EOL [release branches](https://github.com/wagerr/wagerr/branches/all) and create a tag `v${branch_name}-final`.

  - Delete ["Needs backport" labels](https://github.com/wagerr/wagerr/labels?q=backport) for non-existing branches.

  - wagerr.org RPC documentation update

      - Install [golang](https://golang.org/doc/install)

      - Install the new Wagerr release

      - Run wagerrd on regtest

      - Clone the [wagerr.org repository](https://github.com/wagerr/wagerr.org)

      - Run: `go run generate.go` while being in `contrib/doc-gen` folder, and with wagerr-cli in PATH

      - Add the generated files to git

  - Update packaging repo

      - Push the flatpak to flathub, e.g. https://github.com/flathub/org.wagerr.wagerr-qt/pull/2

      - Push the snap, see https://github.com/wagerr/packaging/blob/master/snap/build.md

  - This repo

      - Archive the release notes for the new version to `doc/release-notes/` (branch `master` and branch of the release)

      - Create a [new GitHub release](https://github.com/wagerr/wagerr/releases/new) with a link to the archived release notes

- Announce the release:

  - wagerr-dev and wagerr-dev mailing list

  - Wagerr announcements list https://wagerr.org/en/list/announcements/join/

  - Wagerr Twitter https://twitter.com/wagerrorg

  - Celebrate
