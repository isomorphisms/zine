# zine

A reader/discovery experiment built from journals, small presses, record labels, and the people who choose what they publish.

The first step is deliberately boring: collect the selectors and their catalogs before designing the reader around them.

## Seed catalogs

- `journals.tsv` — literary magazines and journals
- `presses.tsv` — independent and small book publishers
- `labels.tsv` — record labels with strong curatorial identities
- `discovery.md` — why the selector itself is a useful discovery link

The important edge is not merely “more like this.” A work can lead to the journal, editor, press, label, curator, or publisher that chose it, and from there to other work chosen by the same people.

Historical sources stay in the graph. A dead magazine, press, or label can still be a useful record of taste.

The catalogs are intentionally broader than “independent.” Small human-scale selectors are the center of gravity, but larger institutions can be useful comparison sources too. `status` is checked at a point in time, not a promise that an organization will stay active.
