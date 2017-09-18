# RFC Process

## What's an RFC?

The name itself is a reference to the IETF's Request For Comments process, and
basically involves a document or series of documents which are drafted,
reviewed, and eventually ratified (approved) by the Ayo.js community through
discussion among those interested.

An RFC can extend, modify, or alter any part of the Ayo.js project's code or
documents, whether or not they've been previously documented. This includes
proposing new features, proposing major breaking changes, changes to the Code of
Conduct and other community policies, changes to the governance structure of the
project, and anything else that affects a significant part of the community. It
can also propose entirely new policies and community agreements.

## How do I create an RFC?

* Go to https://github.com/ayojs/ayo
* Create a PR with a new RFC document:
  * Must be in the `rfcs/` directory in the root of the main repo.
  * Must be an `.md` file.
  * The first line should be `# RFC - <title of RFC>`
* Tag at least one member of any relevant teams. If you're not sure who would be
  responsible for reviewing your RFC, let the triage process take care of it for
  you.
* For small changes or moderate policy documentation changes, you may include
  the actual patch along with the main RFC. For example, if you're rewording
  something in the CoC, it might be appropriate to patch the CoC in the same PR
  as the RFC submission.

## How does review work?

When you review an RFC, do so through comments and the review feature on GitHub.
The RFC will not be accepted until there is, preferably, general consensus. If
this can't be reached, it's up to the team or teams responsible for the
subsystem the RFC is about to resolve the conflict according to their own
policies around decisions like these.

Side discussion can happen anywhere, in public or in private, but the official
project channels are the preferred place to talk through things outside of PR
reviews/comments. Furthermore, any decisions/suggestions that are decided on
externally should be recorded in the RFC itself, for record keeping.

## How do RFCs get ratified?

RFCs have a minimum 24 hour waiting period before being accepted or rejected.
Once an RFC has been reviewed on Github and at least one member from each
relevant team has signed off on the changes, the PR will be accepted. Depending
on the change, having a parallel PR may be requested by reviewers.

If it's specifically requested, or if team members determine that the topic of
the RFC demands extra attention and care because of its potential impact, an
RFC's "ratification period" may be extended for as long as the participants and
WG members feel is a reasonable length of time for consideration, at their
discretion.

The RFC may be rejected altogether at the discretion of team members responsible
for the given subsystem, or based on consensus among Ayo.js members affected by
the change.

## What happens after ratification?

Once an RFC is ratified, it is considered Official, and will be marked as
ratified. Any related changes should either be merged directly with the RFC PR,
or done elsewhere after ratification.

## How do I change an RFC after ratification?

RFCs themselves cannot be modified after ratification, but new RFCs can be
proposed and ratified to amend or remove a policy previously ratified through
the RFC process. These amendments will involve the exact same process as a
regular RFC.
