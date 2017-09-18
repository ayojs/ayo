# RFC - New Code of Conduct and Moderation Process

So far, Ayo.js has defaulted to using [Node.js' current Code of
Conduct](https://github.com/nodejs/TSC/blob/master/CODE_OF_CONDUCT.md), which is
the standard Contributor Covenant. Furthermore, it has its own [Moderation
Policy](https://github.com/nodejs/TSC/blob/master/Moderation-Policy.md). This
RFC is a proposal to change the base Code of Conduct for the project, as well as
change and streamline the moderation policy.

### Moderation Policy Values

This RFC aims to change priorities surrounding moderation a bit by changing the
values the Ayo.js project's moderation process will operate based on and then
applying those values into new policy. Roughly stated, the contrasting values
are such:

1. **Moderation is About Empathy** - seeing moderation processes as ways to
"punish" bad behavior results in fear and uncertainty, and ultimately gets in
the way of peaceful, positive resolution of conflicts.

2. **Moderation is About Cohesion** - It's not about making value judgment or
fighting political wars. Instead, it should be seen as a community effort in
finding a greatest-common-denominator under which project members can coexist
and cooperate respectfully with each other. All of us.

3. **Moderation is About Growth** - Most people mean well, even when they're
angry. Moderation should take this into account and focus on helping everyone
involved grow together. Banning someone from the community should be heavily
scrutinized as a potential, serious failure of process, not as "deserved".

4. **Moderation Cannot Be Neutral** - As in, even if it were to try, it would
fail. When someone is sufficiently harmed, moderators are faced with a choice of
who gets to stay: The person who clearly harmed someone, or the person who was
hurt. A just system will not ignore this and will prioritize the needs of those
who were harmed over the comfort of those who caused it.

5. **Moderation Begins With Collaborators** - As much as possible, any
moderation action should begin with individual contributor involvement.
Moderators should be considered a relative extreme when it comes to conflict,
and any contributor should be able to say "hey, the way you're saying this goes
against our guidelines", and it is reasonable to expect the other person to
simply edit their message, offer a brief apology, and allow everyone to move on.
This interaction should be common, comfortable, and rarely lead to escalation.

6. **Trust The Ouch** - It's completely okay if you don't understand someone's
reaction to something you said. If they say it hurts, start from the assumption
that they are being honest and mean well, and offer your cooperation in
adjusting what needs to be adjusted. We can never fully understand everyone
around us. We can never learn every little thing that might upset or harm
others. What we **can** do is listen when they tell you so, and cooperate with
them from there. You're not a bad person for a habit slipping or unintentionally
crossing a line you didn't know about.

7. **Protection, Not Safety** - A project at any significant scale cannot be a
safe space. Something, at some point, will happen, and it's important to make it
clear to current and potential collaborators that this cannot be prevented.
Instead, the project should be committed to going through reasonable,
responsible processes whenever something **does** happen to alleviate the
damage, and to make it clear to others that violations will be taken seriously.
That is really the best a project would be able to offer.

### Moderation Policy Changes

Concretely, moving to a policy that the author believes fulfills these values
involves a few steps:

1. Replace the current code of conduct with one based on the [WeAllJS Code of
Conduct](https://npm.im/weallbehave), which has a built-in enforcement process
that promotes individual interactions, trusting the "ouch", and prioritizes the
hurt over those who caused the hurt when the enforcement process is applied.

2. Remove the existing Moderation Policy, since moderation details are now built
into the CoC.

3. Update relevant contact/reporting information for the moderation team to the CoC.

4. Add a detail about using `<describe>` and `<summary>` when editing posts by
collaborators, instead of editing in place, as well as providing a rational for
the edit, and signing it.

5. Add an event-tracking process that includes methods for tracking details
about enforcement measures in private (managed by the Moderation Team), as well
as publishing a scrubbed version of that log that is available to the public, as
well as establishing a process for non-Moderators to request information kept in
the private log. (Note: This might be worth having as its own RFC, but I think
it's important)

6. Specify the Moderation Team as the ultimate arbiter of moderation actions,
but that changes to the moderation policy should be proposed through the
standard RFC process, and must be ratified by both the Core and Moderation
teams, at least, as well as by any other Team who believes the changes will
affect them.
