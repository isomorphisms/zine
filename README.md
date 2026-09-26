# zine

A small reader/discovery project for contemporary literature on the web.

The first step is deliberately boring: collect publications before designing the reader around them.

`journals.tsv` is the seed catalog. It records the publication, URL, current/historical status, broad forms published, and a short note. `status` is a checked-at-a-point-in-time field, not a promise that a magazine will stay alive forever.

The catalog is intentionally broader than "independent": independent and little magazines are the center of gravity, but institutional journals can be useful comparison sources too.

## Seed poems

`poems.tsv` indexes a small bundled corpus for the first Android reader experiment. The initial texts are early-twentieth-century poems whose cited source editions are public domain in the United States.

The poem body lives in `poems/`; title, author, source, and availability live in `poems.tsv`. Keep those separate so the reader does not have to bake presentation or navigation into the poem text.

`sequence.tsv` is deliberately separate from the corpus. It is only a provisional order for exercising transitions between poems of different lengths and shapes.

The reader rule is simple: one poem at a time, no timer, no automatic advance. Remaining on a poem indefinitely is a valid state. Moving to another poem must be deliberate.
