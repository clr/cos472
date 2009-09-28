#!/usr/bin/ruby

class Mushroom
  
  def self.read( file_name )
    raise "File not found!" unless File.exists?( file_name )
    return File.readlines( file_name ).reject do |line|
      line.match /^(lines|attributes)/
    end.collect do |line|
      line.chomp.split(',')
    end
  end

end


class Value
  attr_reader :count, :letter, :attribute

  def initialize( letter, attribute, class_count, sample_length )
    @count = 1
    @letter = letter.to_sym
    @attribute = attribute
    @class_count = class_count
    @sample_length = sample_length
  end

  def self.for_mushrooms( class_letter, class_attribute, mushrooms )
    result_set = []
    mushrooms.each do |mushroom|
      mushroom.each_with_index do |attribute, i|
        class_count = mushroom[ class_attribute ].to_sym == class_letter ? 1 : 0
        if( value = Value.from( attribute.to_sym, i, result_set ) )
          value.increment( class_count )
        else
          result_set << Value.new( attribute, i, class_count, mushrooms.length )
        end
      end
    end
    return result_set
  end

  def self.from( letter, attribute, result_set )
    result_set.detect{ |r| ( r.letter == letter ) && ( r.attribute == attribute ) } 
  end

  def increment( class_count )
    @count += 1
    @class_count += class_count
  end

end

class Decision 

  def self.create_tree( letter, attribute, mushrooms, available_values )
    match_count = mushrooms.select{ |m| m[ attribute ].to_sym == letter.to_sym }.length
    
    # Return nothing if there is nothing left to split on.  We should never
    # get here, right?
    if( available_values.length == 0 )
      return LeafNode.new( 0 )
    end
    
    # Return a leaf node if all the children have one class or the other.
    if( match_count == 0 )
      return LeafNode.new( -1 )
    end
    if( match_count == mushrooms.length )
      return LeafNode.new( 1 )
    end

    # Split on the next node, and recurse.
    split_value = available_values.pop
    positive_mushrooms = []
    negative_mushrooms = []
    mushrooms.each do |mushroom|
      if mushroom[ split_value.attribute ].to_sym == split_value.letter.to_sym
        positive_mushrooms << mushroom
      else
        negative_mushrooms << mushroom
      end
    end
    return DecisionNode.new(
      split_value,
      letter,
      attribute,
      positive_mushrooms,
      negative_mushrooms,
      available_values
    )
  end
  
end

class LeafNode

  def initialize( is_class )
    @is_class = is_class
  end

  def answer
    case @is_class
    when -1
      'false'
    when 0
      'maybe'
    when 1
      'true'
    end
  end

  def print_tree( tab = 0 )
    puts "#{ ' ' * tab }Decide #{ self.answer }."
  end

end

class DecisionNode

  def initialize( split_value, class_letter, class_attribute, left, right, available_values )
    @value = split_value
    @left = Decision.create_tree( class_letter, class_attribute, left, available_values.clone )
    @right = Decision.create_tree( class_letter, class_attribute, right, available_values.clone )
  end

  def print_tree( tab = 0 )
    puts "#{ ' ' * tab }Split on attribute #{ @value.attribute } letter #{ @value.letter }."
    @left.print_tree( tab + 1 )
    @right.print_tree( tab + 1 )
  end

end

mushrooms = Mushroom.read './mushrooms.data'
tree = Decision.create_tree :e, 0, mushrooms, Value.for_mushrooms( :e, 0, mushrooms )
tree.print_tree
