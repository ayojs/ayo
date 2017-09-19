# Ayo.js

[![Build Status](https://travis-ci.org/ayojs/ayo.svg?style=flat&branch=latest)](https://travis-ci.org/ayojs/ayo?branch=latest)

_(Note: Ayo.js is forked from [Node.js][]. Currently, a lot of the documentation
still points towards the Node.js repository.)_

Ayo.js is a JavaScript runtime built on Chrome's V8 JavaScript engine. It
uses an event-driven, non-blocking I/O model that makes it lightweight and
efficient. Ayo.js, like the rest of the JavaScript
implementations, benefits from the [`npm` package ecosystem][], the largest set
of open source libraries in the world.

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
    * [Core Team](#core-team)
    * [Moderation Team](#moderation-team)

## Resources for Newcomers

### Official Resources

* [Join the Discord channel]!

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

There are no hard and fast rules to determine if a bug is worth reporting as
a security issue. The general rule is any issue worth reporting
must allow an attacker to compromise the confidentiality, integrity
or availability of the Node.js application or its system for which the attacker
does not already have the capability.

To illustrate the point, here are some examples of past issues and what the
Security Reponse Team thinks of them. When in doubt, however, please do send
us a report nonetheless.


### Public disclosure preferred

- [#14519](https://github.com/nodejs/node/issues/14519): _Internal domain
  function can be used to cause segfaults_. Causing program termination using
  either the public Javascript APIs or the private bindings layer APIs requires
  the ability to execute arbitrary Javascript code, which is already the highest
  level of privilege possible.

- [#12141](https://github.com/nodejs/node/pull/12141): _buffer: zero fill
  Buffer(num) by default_. The buffer constructor behaviour was documented,
  but found to be prone to [mis-use](https://snyk.io/blog/exploiting-buffer/).
  It has since been changed, but despite much debate, was not considered misuse
  prone enough to justify fixing in older release lines and breaking our
  API stability contract.

### Private disclosure preferred

- [CVE-2016-7099](https://nodejs.org/en/blog/vulnerability/september-2016-security-releases/):
  _Fix invalid wildcard certificate validation check_. This is a high severity
  defect that would allow a malicious TLS server to serve an invalid wildcard
  certificate for its hostname and be improperly validated by a Node.js client.

- [#5507](https://github.com/nodejs/node/pull/5507): _Fix a defect that makes
  the CacheBleed Attack possible_. Many, though not all, OpenSSL vulnerabilities
  in the TLS/SSL protocols also effect Node.js.

- [CVE-2016-2216](https://nodejs.org/en/blog/vulnerability/february-2016-security-releases/):
  _Fix defects in HTTP header parsing for requests and responses that can allow
  response splitting_. While the impact of this vulnerability is application and
  network dependent, it is remotely exploitable in the HTTP protocol.

When in doubt, please do send us a report.

## Current Project Team Members

The Ayo.js project team is comprised of a core team, a moderation team, and
various sub-teams. For more information, see [GOVERNANCE.md](./GOVERNANCE.md).

### Core Team

| | Github | Name | Email | Pronouns |
| ---| --- | --- | --- | --- |
| ![addaleax](https://avatars2.githubusercontent.com/u/899444?v=4&s=64) | [addaleax](https://github.com/addaleax) | **Anna Henningsen** | anna@addaleax.net | she/her |
| ![aqrln](https://avatars0.githubusercontent.com/u/4923335?v=4&s=64) | [aqrln](https://github.com/aqrln) | **Alexey Orlenko** | eaglexrlnk@gmail.com | he/him |
| ![Harrison-M](https://avatars1.githubusercontent.com/u/1491200?v=4&s=64) | [Harrison-M](https://github.com/Harrison-M) | **Hal Massey** | harrison.massey@gmail.com | he/him |
| ![pup](https://avatars0.githubusercontent.com/u/6078550?v=4&s=64) | [pup](https://github.com/pup) | **Olivia Hugger** | olivia@fastmail.com | she/her |
| ![Qard](https://avatars1.githubusercontent.com/u/205482?v=4&s=64) | [Qard](https://github.com/Qard) | **Stephen Belanger** | admin@stephenbelanger.com | he/they |
| ![sandfox](https://avatars1.githubusercontent.com/u/260919?v=4&s=64) | [sandfox](https://github.com/sandfox) | **James Butler** | james.butler@sandfox.co.u | they/he |
| ![TimothyGu](https://avatars1.githubusercontent.com/u/1538624?v=4&s=64) | [TimothyGu](https://github.com/TimothyGu) | **Timothy Gu** | timothygu99@gmail.com | he/him |
| ![varjmes](https://avatars0.githubusercontent.com/u/542140?v=4&s=64) | [varjmes](https://github.com/varjmes) |  **James Spencer** | hello@jmes.tech | they/them |

### Moderation Team

| | Github | Name | Email | Pronouns |
| ---| --- | --- | --- | --- |
| ![AgentAntelope](https://avatars1.githubusercontent.com/u/1962412?v=4&s=64) | [AgentAntelope](https://github.com/AgentAntelope) | **Fell Sunderland** | agentantelope+github@gmail.com | he/him |
| ![janl](https://avatars1.githubusercontent.com/u/11321?v=4&s=64) | [janl](https://github.com/janl) | **Jan Lehnardt** | jan@apache.org | he/him |
| ![NoahDragon](https://avatars3.githubusercontent.com/u/5704064?v=4&s=64) | [NoahDragon](https://github.com/NoahDragon) | **Abner Chou** | hi@abnerchou.me | he/him |
| ![scotttrinh](https://avatars2.githubusercontent.com/u/1682194?v=4&s=64) | [scotttrinh](https://github.com/scotttrinh) | **Scott Trinh** | scott@scotttrinh.com | he/him |
| ![siddharthkp](https://avatars0.githubusercontent.com/u/1863771?v=4&s=64) | [siddharthkp](https://github.com/siddharthkp) | **Siddharth Kshetrapal** | siddharth.kshetrapal@gmail.com | he/him |
| ![SomeHats](https://avatars1.githubusercontent.com/u/1489520?v=4&s=64) | [SomeHats](https://github.com/SomeHats) | **Alex Dytrych** | alex@dytry.ch | she/they |
| ![srilq](https://avatars3.githubusercontent.com/u/18026180?v=4&s=64) | [srilq](https://github.com/srilq) | **Stef** | stef@srilq.email | they/them |
| ![zkat](https://avatars2.githubusercontent.com/u/17535?v=4&s=64) | [zkat](https://github.com/zkat) | **Kat Marchán** | kzm@sykosomatic.org | they/them |

[Node.js]: https://github.com/nodejs/node
[`npm` package ecosystem]: https://www.npmjs.com
[Join the Discord channel]: https://discord.gg/hCgptwH
[Website]: https://nodejs.org/en/
[Contributing to the project]: CONTRIBUTING.md
[Node.js Help]: https://github.com/nodejs/help
[Node.js Moderation Policy]: https://github.com/nodejs/TSC/blob/master/Moderation-Policy.md
[#node.js on chat.freenode.net]: https://webchat.freenode.net?channels=node.js&uio=d4
[#node-dev on chat.freenode.net]: https://webchat.freenode.net?channels=node-dev&uio=d4
[Code of Conduct]: https://github.com/nodejs/TSC/blob/master/CODE_OF_CONDUCT.md
