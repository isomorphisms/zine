# Discovery by selector

A good work should open more than an author/artist page.

One of the strongest discovery links is the **selector**: the small journal, editor, press, label, curator, translator, or other human-scale institution that chose to put the work into the world.

Examples:

- like a poem -> inspect the journal and its editors -> sample other writers they publish
- like a book -> inspect the press and its editors -> sample the press catalog
- like a record -> inspect the label and the people running it -> sample other releases
- like an artist -> inspect the small labels, journals, venues, presses, compilations, and collaborators around them

This is not the same thing as "more like this." The point is not automated similarity. It is following a human chain of taste.

A tiny label run out of an apartment can be a better discovery index than a genre page because the catalog represents repeated choices made by particular people. The same is true of a tiny press or literary journal.

Inactive institutions remain useful. Their catalogs still record those choices, so historical labels, presses, and magazines should not disappear from the graph merely because they stopped publishing.

## Initial seed examples

- Night People -> Featureless Ghost, among many others
- Drag City -> its catalog and associated artists
- CLASH Books -> Jackie Ess and the rest of the press catalog
- Dorothy -> a deliberately tiny annual list with a very concentrated editorial signal

The source tables are intentionally plain data:

- `journals.tsv`
- `labels.tsv`
- `presses.tsv`

Later code can turn these into edges among works, artists/authors, selectors, and people without forcing a recommendation-feed model onto the reader.
