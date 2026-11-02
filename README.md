# CLI Yahtzee

Simple CLI for yahtzee. Defaults to two players, support for arbitrarily many. Give names as arguments, or default to `Player 1` and `Player 2`

Requires terminal with support for unicode as interface is drawn with non-ASCII characters.

## Usage

Build with `make`, run with `./yahtzee`.

Roll dice with `space`, move with `arrow keys`, lock a die with `enter`, change focus between dies and the scoreboard with `tab`.

## Interface

Looks a bit more refined in an actual terminal, but below is an example of how the CLI looks:

```text
Player 1's turn  Throws left: 2                                           Scorecard
                                                                          ┌──────────────────┬──────────┬──────────┐
╭─         ─╮                                                             │ Score            │ Player 1 │ Player 2 │
  ╭───────╮     ╭───────╮     ╭───────╮     ╭───────╮     ╭───────╮       ├──────────────────┼──────────┼──────────┤
  │ ●   ● │     │ ●     │     │ ●     │     │ ●   ● │     │ ●   ● │       │ Ones             │        0 │          │
  │       │     │       │     │   ●   │     │       │     │ ●   ● │       │ Twos             │        2 │          │
  │ ●   ● │     │     ● │     │     ● │     │ ●   ● │     │ ●   ● │       │ Threes           │        3 │          │
  ╰───────╯     ╰───────╯     ╰───────╯     ╰───────╯     ╰───────╯       │ Fours            │        8 │          │
╰─         ─╯                                                             │ Fives            │        0 │          │
                                                                          │ Sixes            │        6 │          │
                                                                          ├──────────────────┼──────────┼──────────┤
                                                                          │ Sum              │          │          │
                                                                          │ Bonus            │          │          │
                                                                          ├──────────────────┼──────────┼──────────┤
                                                                          │ Three of a Kind  │        0 │          │
                                                                          │ Four of a Kind   │        0 │          │
                                                                          │ Full House       │        0 │          │
                                                                          │ Small Straight   │        0 │          │
                                                                          │ Large Straight   │        0 │          │
                                                                          │ Yahtzee          │        0 │          │
                                                                          │ Chance           │       19 │          │
                                                                          ├──────────────────┼──────────┼──────────┤
                                                                          │ Total            │          │          │
                                                                          └──────────────────┴──────────┴──────────┘
```