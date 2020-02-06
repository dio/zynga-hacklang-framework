<?hh

require_once dirname(__FILE__).'/src/autoload.hh';

use Zynga\Framework\Testing\BootStrap\V2\BootStrap;
use Zynga\Framework\Testing\TestCase\V2\Config\Manager as TestCaseManager;
use Zynga\Framework\Environment\DevelopmentMode\V1\DevelopmentMode;
use Zynga\Framework\Environment\CodePath\V1\CodePath;

// Set the codepath to the root of this codebase.
CodePath::setRoot(dirname(__FILE__));
CodePath::setExternalLibraryRoot(CodePath::getRoot().'/third-party');

// Stand up the bootstrap for all of our tests.
BootStrap::init();

// Detect what development enviornment we are within.
DevelopmentMode::detect();
