# FLOCKING FORMATIONS
 
 ## The idea behind it
 Formations peaked my interest, so naturally I went to research how to form formations inside of a flock.
 The first step I took was seeing what kind of formations I could implement, and how I would approach this.
 Something I knew beforehand was that I wanted to make the agents form inside of a flock. 
 Like pressing a checkbox which would turn on the formations and disabling them again as well, so I'd have my regular flock back.
 
 ## How I started
 
 I started to go to town with some simple math and started with the most simple of formations, a circle.
 After spending some time on paper and Geogebra, I came up with a formula which would calculate the positions of the agents in the circle formation.
 This being
 
 ```
 Xposition = circleRadius * cos(360/NumberOfAgentsInCircle * AgentNumber)
 Xposition = circleRadius * sin(360/NumberOfAgentsInCircle * AgentNumber)
 ```
 Below you'll seen an example of a formation with a **radius of 2** and **6 agents**
 ![AltText](https://cdn.discordapp.com/attachments/720322360330158211/799274524435546172/unknown.png)
 
 Obviously you can change the rafius size and NrAgents accordingly to adapt the circle to your needs. But this layed down the basics.
 
 ## Time to code!
 I booted up the GGP_Framework we got earlier this year since we already had a flock in that, thus I could just program the functionaliy on top of that.
 I started coding the formula to make *targets* the agents would need to see to, to make the formation.
 
 When doing this, I stumbled upon a problem, I didn't know how I would tell certain agents to form a formation with other agents. Sure, they all have a neighbourhood, but that    would not limit the amount of agents there could maximum be in a formation. 
 I started searching on the internet, and came to the conclusion that most formations have this neat little thing called a ***leader***. 
 The other agents would form around the leader, whilst the leader would control how many agents are allowed in his formation.
 
 After implementing a leader, I gave him a vector of targets, those being of where the agents would seek to!
 Show below is what in my program, a leader with it's targets look like:
 
 ![AltText](https://cdn.discordapp.com/attachments/720322360330158211/799277593940459540/unknown.png)
 
 Next up, I told the leader to make the agents within it's radius to be assigned to the formation. Once they're in the radius, the agents swap out their flocking behaviour for a seek towards the target.
 
 ![AltText](https://cdn.discordapp.com/attachments/720322360330158211/799279378814468106/unknown.png)
 
 Now after there are enough agents in it's radius, the leader will have a formation around it, which will follow the leader as long as desired. 
 
 ![AltText](https://cdn.discordapp.com/attachments/720322360330158211/799279883032723496/unknown.png)
 
 The nice thing about this is, whenever something would break up the formation, like other agents bumping into the formation, the agents inside  would just seek back to the formation and forming the circle again!
 Another neat pro is that, in the way that I've set this up, you could add multiple leaders, which all make their own formations by gathering other agents.
 I told the program to save the flocking steering behaviour, so whenever I would turn off the formation, the surrounding agents would leave the formation, and return to it's flock.
 
 ![AltText](https://cdn.discordapp.com/attachments/720322360330158211/799280641027211334/ezgif.com-gif-maker.gif)
 
 ## Problems
 Currently, there are still a lot of problems with my implementation, my implementation is very small, but it does the job. 
 I'll list a few, and how I could solve them:
 
 #### The formation is very unstable
 Because the agents inside the formation use a seek to get to it's targets, it will be incredibly unstable. Online I've found a common way to keep the formation nice an stable  is by using alignment. To keep the agents aligned with it's leader. I tried to implement this was well by blending a seek and alingment, but ran into some issues which I did not have the time for to resolve it. The agents would stop seeking to the targets and just stay in place. 
 
 #### It can't find it's way around walls and/or small passages.
 Because it makes use of a mere simple seek, the agents don't have any proper pathfinding. A neat way to solve this issue woud be by making the agents in the formation make use of proper A* pathfinding towards the targets of the leader. This way they would always reach their destination regardless of what obstacles are in their way.
 
 ## The future
 Now like I mentioned earlier, this is a very small and barebones implementation, I could add a lot of things to make this better, I could keep improving it.
 Things that I could and would like to try in the future would be things like:
 - Adding more formation shapes
 - Use A* pathfinding
 - Use alignment
 
 
