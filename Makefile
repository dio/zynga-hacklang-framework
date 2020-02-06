RUN_IN_DOCKER=./vendor/bin/run-inside-docker

default:
	@echo "--------------------------------------------------------------------------------"
	@echo "Development commands"
	@echo "--------------------------------------------------------------------------------"
	@echo " check-exceptions                     - Verifies the use of sub classed exceptions"
	@echo " check-strict                         - Checks that all files are within guidelines"
	@echo " hack-format                          - Reformats the hack code into facebook standard"
	@echo " hack-restart                         - Restarts the hhvm instance"
	@echo " hack-lint                            - Verifies the codebase is hack compliant"
	@echo " test                                 - Runs tests for the framework, includes"
	@echo "                                        third party tests also"
	@echo " test-framework                       - Runs tests for the framework"
	@echo " test-phpunit                         - Runs tests for phpunit"
	@echo " test-raw                             - Runs phpunit.phar instead of our phpunit"
	@echo "--------------------------------------------------------------------------------"

hack-restart:
	time $(RUN_IN_DOCKER) ./vendor/bin/hhvm-restart-wrapper

hack-lint:
	time $(RUN_IN_DOCKER) hh_client --lint

hack-format:
	find ./include -type f -exec hh_format -i {} \;

check-strict:
	#./bin/check-hack-strict
	echo 'check-strict: disabled for now'

check-exceptions:
	ack 'use \\Exception' include/ | grep -v 'Test.hh'
	ack 'new Exception' include/ | grep -v 'Test.hh'

test: hack-restart hack-lint check-strict test-single-canary test-all
	echo "DONE"

test-single-canary:
	time ./vendor/bin/run-single-test ./src/Zynga/Framework/ELO/V1/Chess/MatchTest.hh

test-all:
	chmod -R 0755 tmp/Disk-IO-ManagerTest ; rm -rf tmp/Disk-IO-ManagerTest
	time ./vendor/bin/phpunit --testsuite framework-tests,phpunit-tests

test-framework:
	time ./vendor/bin/phpunit --testsuite framework-tests

test-phpunit:
	time ./vendor/bin/phpunit --testsuite phpunit-tests

test-xdebug: hack-restart hack-lint check-strict
	time ./vendor/bin/phpunit-xdebug --testsuite tests
