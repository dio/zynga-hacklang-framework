<?hh // strict

namespace Zynga\Framework\ReflectionCache\V1;

use \Exception;
use \ReflectionClass;

class ReflectionClasses {

  private static Map<string, ?ReflectionClass> $_classes = Map {};

  public static function getReflection(mixed $classOrName): ?ReflectionClass {

    $className = '';

    if (is_string($classOrName)) {
      $className = $classOrName;
    } else if (is_object($classOrName)) {
      $className = get_class($classOrName);
    } else {
      $className = strval($classOrName);
    }

    try {

      $reflected = self::$_classes->get($className);

      if ($reflected instanceof ReflectionClass) {
        return $reflected;
      }

      // --
      // attempt to find the class in the interpreter, allow the autoloader to
      // pick up the class if needed.
      // --
      if (class_exists($className, true) !== true) {
        return null;
      }

      // The one and only ReflectionClass call in the stack.
      $reflected = new ReflectionClass($className);
    } catch (Exception $e) {
      $reflected = null;
    }

    self::$_classes->set($className, $reflected);

    return $reflected;

  }

}
