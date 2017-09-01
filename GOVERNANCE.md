# Ayo.js Project Governance

The Ayo.js governance is split into multiple parts:

* The core team, which is responsible for high-level decisions
* The moderation team, which enforces the Code of Conduct
* Multiple sub-teams, which handle more specific matters like maintaining Ayo.js
  itself or maintaining Ayo.js documentation
* Occasionally, EFA's (Elected Final Arbiters), which serve to break stalemates
  in consensus finding

## Teams

Teams exist in order to steward certain parts of the project.

All team members are elected by method of volunteering. Any contributor to a
team's efforts can volunteer for said team, or be nominated by a team member.
All nominations are publicly reviewed and voted on by the existing team members.

If a team member decides to step down, they can do so instantly. If a team
member is found to be violating the project's Code of Conduct, they will be
moderated and potentially permanently ejected from the team.

There should always be at least 3 members of any team.

There are a number of fixed teams:

#### Core team

The core team is a team with the purpose of deciding the general direction of
the project and managing cross-cutting concerns. They only decide over the most
high-level aspects, all other matters should be delegated to another, more
specific team.

Every member of the core team is also an Owner of the [ayojs]() GitHub
organization.

#### Moderation team

The moderation team serves to enforce the project's Code of Conduct and manage
situations of conflict. There are no core team members on the moderation team,
in order to not pick sides.

Every member of the moderation team is also an Owner of the [ayojs]() GitHub
organization.

#### Sub-teams

Other teams focus on one particular area of the project. They have full autonomy
over said area, their decisions do not need to be approved by any core team
member. Every sub-team needs to have one member that is also a member of the
core team, in order to notice and manage cross-cutting concerns.

Each team decides who gets permissions for their respective
repository/repositories.

## Elected Final Arbiters

Elected Final Arbiters (EFA) are Members of the Ayo.js project that serve to
resolve issues where there is no consensus. They are elected on a per-issue
basis.

There should always be multiple EFAs, a recommended number is 3-5.

EFAs have the authority to establish a decisive vote on issues where no
consensus can be reached by the team(s). They do not place
above the teams in the hierarchy, for they are Members. EFA's should be familiar
with the issue at hand.

## Consensus Seeking Process

The teams and EFAs follow a [Consensus Seeking][] decision making model. This
means that in order to find consensus, nobody in the group has to object. If
no consensus can be reached, the issue can be postponed until the next time
the team reconverges.

[ayojs]: https://github.com/ayojs
[Consensus Seeking]: http://en.wikipedia.org/wiki/Consensus-seeking_decision-making
