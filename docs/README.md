## TODO

1.	(Qt) Implement custom GUI component for Unibet like game-streak, which is a custom chart of sorts. It basically is 5 boxes, 
	containing either the letter "L" or "W" denoting if that game, of the last 5 games (or longer), was won or lost. 
	This box should also have hover capabilities, to trigger an advanced tooltip, basically a full window like tooltip, that 
	will show the statistics of that game, both raw and analyzed data.

	Basically the entire component will look like this:	| ---[L]-[W]-[W]-[L]-[W]---> |
	So everything between the | | will be the canvas of sorts, and the boxes on the canvas will be custom widgets as well,
	so that we can hook signals and slots together with them, provide a mapToPoint/mapToValue functionality to tell the user
	of the widget where they are located on the canvas (mapToPoint), or to what data, a specific widget instantiation 
	is pointing to (mapToValue).

	And when the user hovers over one of the boxes, the related data will be displayed in a custom tooltip like GUI Widget
2. (Qt) Improve on the line charts that Qt provides. They are abhorrent.
	a. Add squares to denote where the datapoints are.
	b. Add custom tooltip functionality besides the "call out", it is much too simple.
3. (Backend) Figure out what data we can scrape that ML can be used to gain new info from. (This might never be possible, so this 
	is the absolute last task, seeing as how I have 0 real world experience).
4. (Qt + Backend) Figure out a way, to visualize trend data. What is useful? What kind of trend data can actually tell us something,
	as to how we should be placing our bets? Perhaps a standard/specification of _all_ the different aspects of a game need
	to be developed first, so one can check off boxes of what has been implemented? Yes, that sounds good.


### Features to add / change

	[x] PDO stats
	[ ] Corsi stats
	[ ] Live Result analysis
	[ ] Adjust EN goal stats, the empty net let ups need to be adjusted to show games where goalie was pulled. If stats say for example (currently) 32%, 
		this might count for games where the goalie was *not* pulled (usually when deficit is greater than 2 goals), therefore, this 
		make the stat not really show the truthyness of how often a goal is let up/made, when it comes to Empty Net scoring.
	[x] Make sure that GF/GA statistics do not account for Shootout goals (which it currently *does*).
	[ ] Distribution analysis of goals made, w/ respect to time. We can create a chart, that displays in what "regions" of game time
		where most goals are scored / let up, instead of just to static "by period" statistic.

### Statistics and Trend specification

Statistics come in many shapes and forms, raw stats, are the following ones:

Special Teams efficiency => 
	PK: 1.0 - (Goals let up / Times in penalty kill)
	PP: Scoring / Powerplay attempts

Shot efficiency		=> Scoring / Shots attempted
Save percentage		=> 1-(Goals let up/Shot attempts against)
Goals for		=> Scoring
Goals against		=> Goals let up
Shots for		=> Shots attempted
Shots against		=> Shot attempts against
Corsi			=>
PDO			=>

For most of these raw stats, one can do per-period analysis over a span of games to determine
in what period on average the teams perform best and worse.
