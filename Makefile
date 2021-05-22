.PHONY: publish

publish:
	git push origin :latest && git tag -f latest && git push origin refs/tags/latest
