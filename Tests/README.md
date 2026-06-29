As part of the rdkservices open source activity and logical grouping of services into various entservices-* repos, the changes to L1 and L2 tests below are effective henceforth.

# Changes Done:
Since the mock part is common across various plugins/repos and common for L1, L2 & etc, the gtest and gmock related stubs (including platform interface mocks) are moved to a new repo called "entservices-testframework" and L1 & L2 test files of each plugin moved to corresponding repos, you can find them inside Tests directory of each entservices-*.
Hence, any modifications/additions related to mocks should be committed to entservices-testframework repo @ rdkcentral and any modifications/additions related to test cases should be committed to Tests directory of corresponding entservices repo.

# Individual Repo Handling
Each individual entservices-* repo includes a YAML file to trigger the L1, L2, and L2-OOP test jobs in GitHub Actions. This YAML file triggers the build jobs below in addition to the regular build jobs (Thunder, ThunderTools, etc.).
~~~
a/ Build mocks => Create the TestMock library from the required mock-related stubs and copy it to install/usr/lib.
b/ Build entservices-<repo-name> => Create the test .so from enabled test sources in the plugin repo.
c/ Build entservices-testframework => Create the L1/L2 executable by linking the plugin test .so files.
~~~
This helps ensure the related plugin repositories remain consistent when new changes are introduced.

##### Steps to run L1, L2, L2-OOP test locally #####
```
1. checkout the entservices-<repo-name> to your working directory in your build machine.
example: git clone https://github.com/rdkcentral/entservices-deviceanddisplay.git

2. switch to entservices-<repo-name> directory
example: cd entservices-deviceanddisplay

3. check and ensure current working branch points to develop
example: git branch

4. Run below curl command to download act executable to your repo.
example: curl -SL https://raw.githubusercontent.com/nektos/act/master/install.sh | bash

5. Run L1, L2, L2-oop test
example: ./bin/act -W .github/workflows/tests-trigger.yml -s GITHUB_TOKEN=<your access token>

NOTE: By default tests-trigger.yml triggers all tests (L1, L2, etc.) in parallel. If you want to trigger/verify only one test, remove the other trigger rules from tests-trigger.yml.
```
# testframework Repo Handling
tf-trigger.yml file of testframework repo will get loaded into github action whenever there is a pull or push happens. This file in-turn triggers all individual repos L1, L2, L2-oop tests. testframework repo test can run only in github workflow.

NOTE:
If you face a secret token related error while running your workflow YAML, please comment out the line below:
#token: ${{ secrets.RDKE_GITHUB_TOKEN }}

# Execution usecases where manual change required before triggering the test:
```
a/ changes in testframework repo only:
Need to change ref pointer of "Checkout entservices-testframework" job in individual repo yml file, to point your current working branch of testframework and in tftrigger.yml of testframework repo need to change trigger branch name to your individual repo branch name instead of develop which is default.
example:
ref: topic/method_1  /* Checkout entservices-testframework job */
uses: rdkcentral/entservices-deviceanddisplay/.github/workflows/L1-tests.yml@topic/method_1 /* tf-trigger.yml */

b/ changes in both testframework repo and individual repo:
Changes mentioned in step (a) above + "Checkout entservices-deviceanddisplay-testframework" job in individual repo yml file, ref field to point your deviceanddisplay current working branch.
example:
ref: topic/method_1 /* Checkout entservices-testframework job */
ref: topic/method_1 /* Checkout entservices-deviceanddisplay-testframework job */
uses: rdkcentral/entservices-deviceanddisplay/.github/workflows/L1-tests.yml@topic/method_1 /* tf-trigger.yml */

c/ changes in individual entservices-* repo only
no changes required
```
