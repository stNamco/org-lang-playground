run: 
	docker run --rm -it -v $(PWD)/org-lang:/home/user/org-lang org-lang-enviorment

.PHONY: run