run: 
	docker run --rm -it -v $(PWD)/org-lang:/home/user/org-lang org-lang-enviorment

format:
	clang-format -i -style=google ./org-lang/*.c

.PHONY: run
