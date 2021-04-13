Re: 
	@./A2 $(n) $(i) $(t) $(s)
	@python3 ../src/format_checker.py $(i) ../output/output_L_$(s)_$(t).txt ../output/output_U_$(s)_$(t).txt

tests:
	@for n in $$(seq 1 50); do ./A2 $(n) $(i) $(t) $(s); done
	@python3 ../src/statistics.py ../util/time_$(s).txt

generate:
	@./a.out > ../inputs/matrix_4.txt