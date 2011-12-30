git.cmd diff $* -- `git.cmd ls-files | grep -vE "pic32.*X|headers|nichelite"`
