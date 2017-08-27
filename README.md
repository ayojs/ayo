# Ayo.js

[![Build Status](https://travis-ci.org/ayojs/ayo.svg?style=flat&branch=master)](https://travis-ci.org/ayojs/ayo?branch=master)

_(Note: Ayo.js is forked from [Node.js](https://nodejs.org). Currently, a lot of the documentation
still points towards the Node.js repository.)_

Ayo.js is a JavaScript runtime built on Chrome's V8 JavaScript engine. It
uses an event-driven, non-blocking I/O model that makes it lightweight and
efficient. Ayo.js shares the [`npm` package ecosystem](https://www.npmjs.com/), which is the largest ecosystem of
open source libraries in the world, with the rest of the free JavaScript implementations.

Contributions, policies, and releases are managed under an
[open governance model](./GOVERNANCE.md).

**This project is bound by a [Code of Conduct][].**

# Table of Contents

* [Resources for Newcomers](#resources-for-newcomers)
* [Release Types](#release-types)
  * [Download](#download)
    * [Current and LTS Releases](#current-and-lts-releases)
    * [Nightly Releases](#nightly-releases)
    * [API Documentation](#api-documentation)
  * [Verifying Binaries](#verifying-binaries)
* [Building Ayo.js](#building-ayojs)
  * [Security](#security)
  * [Current Project Team Members](#current-project-team-members)
    * [CTC (Core Technical Committee)](#ctc-core-technical-committee)
    * [Collaborators](#collaborators)
    * [Release Team](#release-team)

## Resources for Newcomers

### Official Resources

_To be written_

### Unofficial Resources

_To be written_

## Release Types

The Ayo.js project maintains multiple types of releases:

* **Current**: Released from active development branches of this repository,
  versioned by [SemVer](http://semver.org/) and signed by a member of the
  [Release Team](#release-team).
  Code for Current releases is organized in this repository by major version
  number. For example: [v4.x](https://github.com/nodejs/node/tree/v4.x).
  The major version number of Current releases will increment every 6 months
  allowing for breaking changes to be introduced. This happens in April and
  October every year. Current release lines beginning in October each year have
  a maximum support life of 8 months. Current release lines beginning in April
  each year will convert to LTS (see below) after 6 months and receive further
  support for 30 months.
* **LTS**: Releases that receive Long-term Support, with a focus on stability
  and security. Every second Current release line (major version) will become an
  LTS line and receive 18 months of _Active LTS_ support and a further 12
  months of _Maintenance_. LTS release lines are given alphabetically
  ordered codenames, beginning with v4 Argon. LTS releases are less frequent
  and will attempt to maintain consistent major and minor version numbers,
  only incrementing patch version numbers. There are no breaking changes or
  feature additions, except in some special circumstances.
* **Nightly**: Versions of code in this repository on the current Current
  branch, automatically built every 24-hours where changes exist. Use with
  caution.

More information can be found in the [LTS README](https://github.com/nodejs/LTS/).

## Download

_To be written_

### Verifying Binaries

_To be written_

## Building Ayo.js

See [BUILDING.md](BUILDING.md) for instructions on how to build
Ayo.js from source. The document also contains a list of
officially supported platforms.

## Security

_To be written_

## Current Project Team Members

_To be written_

[Node.js]: https://github.com/nodejs/node
[npm]: https://www.npmjs.com
[Website]: https://nodejs.org/en/
[Contributing to the project]: CONTRIBUTING.md
[Node.js Help]: https://github.com/nodejs/help
[Node.js Moderation Policy]: https://github.com/nodejs/TSC/blob/master/Moderation-Policy.md
[#node.js on chat.freenode.net]: https://webchat.freenode.net?channels=node.js&uio=d4
[#node-dev on chat.freenode.net]: https://webchat.freenode.net?channels=node-dev&uio=d4
[Code of Conduct]: https://github.com/nodejs/TSC/blob/master/CODE_OF_CONDUCT.md
