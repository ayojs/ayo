# RFC - Moderation Action Log

### Introduction

In the interest of openness and transparency, this RFC proposes that the
Moderation Team be required to keep a **public** log of actions taken whenever
they become involved in various situations. An important reason for this is to
increase community confidence in the moderation process, as well as act as
concrete documentation whenever disagreement or confusion happens about whether
the moderation actions taken were correct, or whether the process should be
changed after running into a particular situation or pattern of situations. This
must also be balanced with other concerns, such as privacy, and should be
designed such that it does not lead to the harassment or defamation of anyone
involved.

### Summary of RFC Actions

* Create a moderation log spreadsheet with the required columns.
* Grant full public read access to anyone with the link.
* Grant full edit access to Moderation Team members.
* Add a link to the log directly to `CODE_OF_CONDUCT.md`.

### Keeping a Balance

This RFC intends to balance the following specific concerns:

* **Openness and Transparency**: people should have a good idea of exactly how
  often and in what sorts of situations the Moderation Team has gotten involved.
  This is critical for building trust and confidence in the process.
* **Recordkeeping**: For situations such as repeated violations (or lack
  thereof), or future analysis that can help the moderation process improve or
  change based on actual data.
* **Privacy/Sensitivity**: Some details about moderation are often private, and
  should stay that way.
* **Prevent Further Harm**: The log structure must take specific measures to
  prevent abuse or harassment due to information contained therein, and reassure
  community members about the potential repercussions of such a public record of
  actions they might find themselves involved in.

### Log Structure

The log should be a spreadsheet with public read access, and edit access only
available to active members of the Moderation Team. Any time an event happens,
the log should be updated as soon as possible by the Moderation Team members
involved.

The log will include the following columns:

* **Date** - date of the *enforcement action* (not the event itself)
* **Action Taken** - official action taken by the moderation team, in accordance
  with the moderation process.
* **Venue** - Where the event that triggered moderation happened: Github,
  Discord, Conference, etc.
* **Moderator(s)** - Moderation Team member or members who took the action.
* **URL** - If the event happened on a URL-linkable **PUBLIC** location (such as
  Github), the URL of the issue or issue comment in question.
* **Details** - Freeform description of the event, with specific parties
  omitted. This should be as dry and neutral as possible. This should use
  placeholders instead of names when referring to any involved parties except
  Moderators unless the event is publicly documented elsewhere (such as on
  GitHub).

### Amending/Reversing Decisions

This is not an accusation log, but a concrete record of things the team actually
did. If a decision is reversed, a **new entry** should be added, clarifying the
reversal. A note may be added to the original entry to clarify that it was
reversed, and referencing the line on the spreadsheet with the details.

The log should be treated as append-only in as many cases as possible, with
minor edits as needed, and as specified in this RFC.

### Scrubbing the Log

In some cases, log entries may include personal details or other information
which may unfairly target one or more of the people involved. In these cases,
general community members may contact the Moderation Team through either public
or private channels as specified in the Code of Conduct's contact section to
request a log line be scrubbed of details.

When a log line is scrubbed, the URL and Details field should be cleared and the
Details field replaced with `[scrubbed on request]`. Unless circumstances are
considered to be extreme enough, the rest of the line, including the date, the
action, the venue, and the involved moderators, should be left intact. The
requestor may also ask that only certain details be scrubbed from the
description.

Log lines should only be scrubbed when there is reason to believe that its
existence would:

1. Cause harm to a community member, either physical, emotional, or career-wise.
2. Could be considered defamatory.
3. Reveals private details or information.
4. Incite targeted harassment in any way to any of the involved parties.
5. Cause legal issues for Ayo.js or its members -- such as when legally
compelled to do so, or when that is a reasonable possibility.

There are some exception where even with the above, a scrub request may be
refused, at the discretion of the Moderation Team:

1. The violation by the subject of moderation is extreme enough that they can be
considered a risk to the community. Concretely: physical violence, sexual
assault, large-scale inciting of targeted harassment campaigns, etc.
2. The edit requested would result in falsification of the log - that is, lying
about what actually happened.
3. The requestor is a third party uninvolved and unconnected to anyone the log
refers to. Requests may only be submitted by participants or their
representatives.

The Moderation Team reserves the right to scrub any existing lines at will,
based on team consensus, at their best judgment. Unless the matter is
absolutely extreme, the log entry must retain basic information about events
as mentioned above.
