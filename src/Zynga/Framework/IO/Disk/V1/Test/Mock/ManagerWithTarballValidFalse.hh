<?hh // strict

namespace Zynga\Framework\IO\Disk\V1\Test\Mock;

use Zynga\Framework\IO\Disk\V1\Manager\UNIX as BaseManager;

class ManagerWithTarballValidFalse extends BaseManager {

  <<__Override>>
  public function isTarballValid(string $fileName): bool {
    return false;
  }

}
